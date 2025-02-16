#include "Paintly.h"

#include <math.h>

#include "Convolution.h"

#include "ImpressionistDoc.h"
#include "ImpressionistUI.h"

#include <stdlib.h>
#include <time.h>
#include <iostream>

#include "Bitmap.h"

Paintly::Paintly(ImpressionistDoc* pDoc)
: m_ucRefImage(NULL), width(0), height(0), m_pDoc(pDoc), m_pllMax(NULL)
{
}

Paintly::~Paintly()
{
    delete m_ucRefImage;
    delete m_pllMax;
}

Pointll::Pointll(int x, int y)
{
	xy = Point(x, y);
	next = nullptr;
}

Pointll::~Pointll()
{
}

void Paintly::genRefImage(unsigned char* bmp, int width, int height, int size, double sigma)
{
    this->width = width;
    this->height = height;

    Convolution* con = new Convolution(bmp, width, height);
    Matrix* gaussian = Matrix::GaussianBlurMatrix((size - 1) / 2, sigma);

    if (m_ucRefImage) delete m_ucRefImage;

    m_ucRefImage = new unsigned char[width * height * 3];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            double color[3] = { 0.0, 0.0, 0.0 };
            con->ConvolutionFilter(*gaussian, j, i, false, true, color);
            for (int k = 0; k < 3; k++)
            {
                m_ucRefImage[(i * width + j) * 3 + k] = int(color[k]);
            }
        }
    }

    delete gaussian;
    delete con;
}


void Paintly::DrawPaintly()
{
	m_pDoc->setBrushType(2);
	srand(time(NULL));

	//unsigned char* canvas = new unsigned char[m_nDrawWidth * m_nDrawHeight * 3];
	//memset (canvas, 0, m_nDrawWidth * m_nDrawHeight * 3);

	memset (m_pDoc->m_ucPainting, 0, width * height * 3);
	unsigned char* canvas = m_pDoc->m_ucPainting;
	unsigned char* sourceImage = m_pDoc->m_ucBitmap;

	int size = pow(2, m_pDoc->m_nPaintlyR0Level + 1);

	genRefImage(m_pDoc->m_ucBitmap, m_pDoc->m_nPaintWidth, m_pDoc->m_nPaintHeight, size, 0.5);
	writeBMP("ref.bmp", width, height, m_ucRefImage);

	for (int layer = 0; layer < m_pDoc->m_nPaintlyLayer; ++layer)
	{
		for(int time = 0; time < 1; ++time)
		{
			m_nNumMax = 0;
			//m_pDoc->setSize(size);

			//m_ucRefImage = sourceImage;

			paintLayer(canvas, m_ucRefImage, size, layer, time);

			Point* PointList = new Point[m_nNumMax];
			Point temp_pt;
			Pointll* head = m_pllMax;
			int index;
			for(int t = 0; t < m_nNumMax; ++t)
			{
				PointList[t] = head->xy;
				head = head->next;
			}

			for(int t = 0; t < m_nNumMax; ++t)
			{
				index = rand() % m_nNumMax;
				temp_pt = PointList[index];
				PointList[index] = PointList[t];
				PointList[t] = temp_pt;
			}

			glDrawBuffer(GL_FRONT_AND_BACK);
			m_pDoc->setSize(size);
			for(int t = 0; t < m_nNumMax; ++t)
				m_pDoc->m_pCurrentBrush->BrushMove( PointList[t], PointList[t] );
			glFlush();
			m_pDoc->m_pUI->m_paintView->SaveCurrentContent();
			m_pDoc->m_pUI->m_paintView->RestoreContent();
			//delete m_pllMax;
			m_pllMax = nullptr;
		}
		size /= 2;
	}

	writeBMP("save.bmp", width, height, canvas);
	m_pDoc->m_pUI->m_paintView->RestoreContent();
	printf("finish");
}


void Paintly::paintLayer(unsigned char* canvas, unsigned char* m_ucRefImage, int R, int layer, int time)
{
	double* D = new double[width*height];
	Point max;
	GLubyte color1[3], color2[3];
	double grid = m_pDoc->m_nPaintlyGridSize * R;
	Pointll* temp;

	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			memcpy(color1, canvas + 3 * (x + y * width), 3);
			memcpy(color2, m_ucRefImage + 3 * (x + y * width), 3);
			D[x+y*width] = ColorDiff(color1, color2);
		}

	for (int y = 0; y < height; y += grid)
		for (int x = 0; x < width; x += grid)
		{
			max = Point(x, y);
			if (AreaError(x, y, grid / 2, max, D) > m_pDoc->m_nPaintlyTheshold || (layer == 0 && time == 0))
			{	
				if (!m_pllMax)
					m_pllMax = new Pointll(max.x, max.y);
				else
				{
					temp = m_pllMax;
					while(temp->next)
						temp = temp->next;
					temp->next = new Pointll(max.x, max.y);
				}
				++m_nNumMax;
				printf("%d %d\n",max.x, max.y);
			}
		}
	delete []D;
}


void Paintly::makeSplineStroke(int x_0, int y_0, int R, unsigned char* m_ucRefImage)
{
}

double Paintly::AreaError(int x, int y, double grid, Point& max, double* D)
{
	int upper_x = (x + grid > width) ? width : x + grid,
		lower_x = (x - grid < 0) ? 0 : x - grid,
		upper_y = (y + grid > height) ? height : y + grid,
		lower_y = (y - grid < 0) ? 0 : y - grid;
	double error = 0;
	double nMax = 0;

	for(int j = lower_y; j < upper_y; ++j)
		for(int i = lower_x; i < upper_x; ++i)
		{
			error += D[i+j*width];
			if (D[i+j*width] > nMax)
			{
				nMax = D[i + j * width];
				max = Point(i, j);
			}
		}
	return error / (4 * grid * grid);
}

double Paintly::ColorDiff(GLubyte* color1, GLubyte* color2)
{
	return sqrt(pow(*(color1) - *(color2), 2) + pow(*(color1+1) - *(color2+1), 2) + pow(*(color1+2) - *(color2+2), 2));
}

GLubyte* Paintly::GetColor(unsigned char* source, int x, int y )
{
	if ( x < 0 ) 
		x = 0;
	else if ( x >= width ) 
		x = width-1;

	if ( y < 0 ) 
		y = 0;
	else if ( y >= height ) 
		y = height-1;

	return (GLubyte*)(source + 3 * (y*height + x));
}