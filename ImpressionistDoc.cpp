// 
// impressionistDoc.cpp
//
// It basically maintain the bitmap for answering the color query from the brush.
// It also acts as the bridge between brushes and UI (including views)
//

#include <FL/fl_ask.H>

#include "ImpressionistDoc.h"
#include "ImpressionistUI.h"

#include <math.h>
#include <iostream>

#include "ImpBrush.h"
#include "StrokeDirection.h"
#include "Convolution.h"
#include "ImageProcess.h"
#include "ThreeDTree.h"
#include "Paintly.h"

// Include individual brush headers here.
#include "Brushes.h"

#define DESTROY(p)	{  if ((p)!=NULL) {delete [] p; p=NULL; } }

ImpressionistDoc::ImpressionistDoc() 
{
	// Initialize image name to empty.
	m_imageName[0]	='\0';	

	m_nWidth			= -1;
	m_ucBitmap			= NULL;
	m_ucPainting		= NULL;
	m_ucFadePainting	= NULL;
	m_ucOriginal		= NULL;
	m_ucTemp			= NULL;
	m_ucEdge			= NULL;
	m_ucAnotherImage	= NULL;


	// create one instance of each brush
	ImpBrush::c_nBrushCount	= NUM_BRUSH_TYPE;
	ImpBrush::c_pBrushes	= new ImpBrush* [ImpBrush::c_nBrushCount];

	ImpBrush::c_pBrushes[BRUSH_POINTS]	= new PointBrush( this, "Points" );

	// Note: You should implement these 5 brushes.  They are set the same (PointBrush) for now
	ImpBrush::c_pBrushes[BRUSH_LINES]			
		= new LineBrush(this, "Lines");
	ImpBrush::c_pBrushes[BRUSH_CIRCLES]
		= new CircleBrush(this, "Circles");
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_POINTS]
		= new ScatteredPointBrush(this, "Scattered Points");
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_LINES]
		= new ScatteredLineBrush(this, "Scattered Lines");
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_CIRCLES]
		= new ScatteredCircleBrush(this, "Scattered Circles");
	ImpBrush::c_pBrushes[BRUSH_BLUR]
		= new BlurBrush(this, "Blurring");
	ImpBrush::c_pBrushes[BRUSH_SHARP]
		= new SharpBrush(this, "Sharpening");

	// make one of the brushes current
	m_pCurrentBrush	= ImpBrush::c_pBrushes[0];

	m_pStrokeDirection = new StrokeDirection();
	m_nStrokeType = 0;

	m_pPaintly = new Paintly(this);
}

ImpressionistDoc::~ImpressionistDoc()
{
	delete m_pPaintly;
}


//---------------------------------------------------------
// Set the current UI 
//---------------------------------------------------------
void ImpressionistDoc::setUI(ImpressionistUI* ui) 
{
	m_pUI	= ui;
}

//---------------------------------------------------------
// Returns the active picture/painting name
//---------------------------------------------------------
char* ImpressionistDoc::getImageName() 
{
	return m_imageName;
}

//---------------------------------------------------------
// Called by the UI when the user changes the brush type.
// type: one of the defined brush types.
//---------------------------------------------------------
void ImpressionistDoc::setBrushType(int type)
{
	m_pCurrentBrush	= ImpBrush::c_pBrushes[type];
}

//---------------------------------------------------------
// Called by the UI when the user changes the stroke type.
// type: one of the defined stoke types.
//---------------------------------------------------------
void ImpressionistDoc::setStrokeType(int type)
{
	m_nStrokeType = type;
}

//---------------------------------------------------------
// Returns the size of the brush.
//---------------------------------------------------------
int ImpressionistDoc::getSize()
{
	return m_pUI->getSize();
}

//---------------------------------------------------------
// Sets the size of the brush.
//---------------------------------------------------------
void ImpressionistDoc::setSize(int size)
{
	m_pUI->setSize(size);
}

//---------------------------------------------------------
// Returns the width of the brush.
//---------------------------------------------------------
int ImpressionistDoc::getWidth()
{
	return m_pUI->getWidth();
}

//---------------------------------------------------------
// Sets the width of the brush.
//---------------------------------------------------------
void ImpressionistDoc::setWidth(int width)
{
	m_pUI->setWidth(width);
}

//---------------------------------------------------------
// Returns the angle of the brush.
//---------------------------------------------------------
int ImpressionistDoc::getAngle()
{
	return m_pUI->getAngle();
}

//---------------------------------------------------------
// Sets the size of the brush.
//---------------------------------------------------------
void ImpressionistDoc::setAngle(int angle)
{
	m_pUI->setAngle(angle);
}

//---------------------------------------------------------
// Returns the alpha of the brush.
//---------------------------------------------------------
double ImpressionistDoc::getAlpha()
{
	return m_pUI->getAlpha();
}

//---------------------------------------------------------
// Sets the size of the brush.
//---------------------------------------------------------
void ImpressionistDoc::setAlpha(double alpha)
{
	m_pUI->setAlpha(alpha);
}

int ImpressionistDoc::getEdgeClipping()
{
	return m_pUI->getEdgeClipping();
}

void ImpressionistDoc::setEdgeClipping(int value)
{
	m_pUI->setEdgeClipping(value);
}

//---------------------------------------------------------
// Returns the edge threashold.
//---------------------------------------------------------
int ImpressionistDoc::getEdgeThreashold()
{
	return m_pUI->getEdgeThreashold();
}

//---------------------------------------------------------
// Sets the edge threashold.
//---------------------------------------------------------
void ImpressionistDoc::setEdgeThreashold(int threashold)
{
	m_pUI->setEdgeThreashold(threashold);
}

int ImpressionistDoc::getBlurSharpLevel()
{
	return m_pUI->getBlurSharpLevel();
}

void ImpressionistDoc::setBlurSharpLevel(int level)
{
	m_pUI->setBlurSharpLevel(level);
}

//---------------------------------------------------------
// Load the specified image
// This is called by the UI when the load image button is 
// pressed.
//---------------------------------------------------------
int ImpressionistDoc::loadImage(char *iname) 
{
	// try to open the image to read
	unsigned char*	data;
	int				width, 
					height;

	if ((data=readBMP(iname, width, height))==NULL) 
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	// reflect the fact of loading the new image
	m_nWidth		= width;
	m_nPaintWidth	= width;
	m_nHeight		= height;
	m_nPaintHeight	= height;

	// release old storage
	if ( m_ucBitmap ) delete [] m_ucBitmap;
	if ( m_ucPainting ) delete [] m_ucPainting;
	if ( m_ucEdge ) delete [] m_ucEdge;
	if ( m_ucAnotherImage ) delete [] m_ucAnotherImage;
	if (m_ucFadePainting)
	{
		delete[] m_ucFadePainting;
		m_ucFadePainting = new unsigned char[m_nPaintWidth * m_nPaintHeight * 3];
		memset(m_ucFadePainting, 0, m_nPaintWidth * m_nPaintHeight * 3);
	}
	
	m_ucBitmap = NULL;
	m_ucPainting = NULL;
	m_ucOriginal = NULL;
	m_ucEdge = NULL;
	m_ucAnotherImage = NULL;

	m_ucBitmap		= data;

	// allocate space for draw view
	m_ucPainting	= new unsigned char [width*height*3];
	memset(m_ucPainting, 0, width*height*3);

	m_pUI->m_mainWindow->resize(m_pUI->m_mainWindow->x(), 
								m_pUI->m_mainWindow->y(), 
								width*2, 
								height+25);

	edgeDetection();
	m_ucOriginal = m_ucBitmap;

	// display it on origView
	m_pUI->m_origView->resizeWindow(width, height);	
	m_pUI->m_origView->refresh();

	// refresh paint view as well
	m_pUI->m_paintView->resizeWindow(width, height);	
	m_pUI->m_paintView->refresh();

	m_pUI->m_paintView->CreateSavedPhoto();
	m_pPaintly->genRefImage(m_ucBitmap, width, height, getSize());

	return 1;
}


//----------------------------------------------------------------
// Save the specified image
// This is called by the UI when the save image menu button is 
// pressed.
//----------------------------------------------------------------
int ImpressionistDoc::saveImage(char *iname) 
{

	writeBMP(iname, m_nPaintWidth, m_nPaintHeight, m_ucPainting);

	return 1;
}

int ImpressionistDoc::edgeImage(char *iname)
{
	unsigned char*	data;
	int				width,
					height;
	
	if ((data=readBMP(iname, width, height)) == NULL)
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	if (m_pUI->m_origView->isSameSize(width, height))
	{
		if ( m_ucEdge ) delete[] m_ucEdge;

		m_ucEdge = data;
		m_ucOriginal = m_ucEdge;
	}

	refresh();

	return 1;
}

int ImpressionistDoc::anotherImage(char *iname)
{
	unsigned char*	data;
	int				width,
					height;
	
	if ((data=readBMP(iname, width, height)) == NULL)
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	if (m_pUI->m_origView->isSameSize(width, height))
	{
		if ( m_ucAnotherImage ) delete[] m_ucAnotherImage;

		m_ucAnotherImage = data;
	}

	return 1;
}




int ImpressionistDoc::dissolve(char *iname) 
{
	unsigned char*	data;
	int				new_width, 
					new_height;

	if ((data=readBMP(iname, new_width, new_height))==NULL) 
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	int old_width = m_nWidth,
		old_height = m_nHeight;

	if (old_width != new_width || old_height != new_height)
	{
		fl_alert("Please chose an image with same size");
		return 0;
	}

	int width = max(old_width, new_width),
		height = max(old_height, new_height);

	if (width != old_width || height != old_height)
	{
		if (m_ucAnotherImage)
		{
			delete m_ucAnotherImage;
			m_ucAnotherImage = NULL;
		}
	}

	m_nWidth		= width;
	m_nPaintWidth	= width;
	m_nHeight		= height;
	m_nPaintHeight	= height;

	unsigned char* old_original = m_ucBitmap;
	unsigned char* old_painting = m_ucPainting;

	m_ucBitmap = new unsigned char [width*height*3];
	memset(m_ucBitmap, 0, width*height*3);
	m_ucOriginal = m_ucBitmap;

	m_ucPainting = new unsigned char [width*height*3];
	memset(m_ucPainting, 0, width*height*3);

	m_pUI->m_mainWindow->resize(m_pUI->m_mainWindow->x(), 
								m_pUI->m_mainWindow->y(), 
								width*2, 
								height+25);

	m_pUI->m_origView->resizeWindow(width, height);	
	m_pUI->m_origView->refresh();

	m_pUI->m_paintView->resizeWindow(width, height);
	if (m_pUI->m_paintView->getIsFade())
	{
		m_pUI->m_paintView->setIsFade(0);
		m_pUI->m_paintView->refresh();
		m_pUI->m_paintView->setIsFade(1);
	}
	else
		m_pUI->m_paintView->refresh();

	glDrawBuffer(GL_FRONT_AND_BACK);
	glRasterPos2i( 0, 0);

	m_pUI->m_origView->draw_fade( old_width, old_height, new_width,  new_height, data, old_original);

	m_pUI->m_paintView->draw_fade( old_width,  old_height, old_painting);

	edgeDetection();
	m_ucOriginal = m_ucBitmap;
	refresh();

	return 1;
}

//----------------------------------------------------------------
// Clear the drawing canvas
// This is called by the UI when the clear canvas menu item is 
// chosen
//-----------------------------------------------------------------
int ImpressionistDoc::clearCanvas() 
{

	// Release old storage
	if ( m_ucPainting ) 
	{
		delete [] m_ucPainting;

		// allocate space for draw view
		m_ucPainting	= new unsigned char [m_nPaintWidth*m_nPaintHeight*3];
		memset(m_ucPainting, 0, m_nPaintWidth*m_nPaintHeight*3);

		if (m_ucFadePainting)
		{
			delete[] m_ucFadePainting;
			m_ucFadePainting = new unsigned char[m_nPaintWidth * m_nPaintHeight * 3];
			memset(m_ucFadePainting, 0, m_nPaintWidth * m_nPaintHeight * 3);
		}

		// refresh paint view as well	
		m_pUI->m_paintView->refresh();
	}
	
	return 0;
}

void ImpressionistDoc::loadForUndo()
{
	m_pUI->m_paintView->LoadForUndo();
}

void ImpressionistDoc::swap()
{
	if (m_ucBitmap != NULL)
	{
		if (m_pUI->m_paintView->getIsFade())
		{
			m_ucTemp = m_ucOriginal;
			m_ucOriginal = m_ucFadePainting;
			m_ucFadePainting = m_ucTemp;
			m_ucBitmap = m_ucOriginal;
			memset(m_ucPainting, 0, m_nPaintWidth*m_nPaintHeight*3);
		}
		else
		{
			if (m_ucOriginal == m_ucBitmap)
				m_ucOriginal = m_ucPainting;

			m_ucTemp = m_ucPainting;
			m_ucPainting = m_ucBitmap;
			m_ucBitmap = m_ucTemp;
		}
		m_pUI->m_origView->redraw();
		m_pUI->m_paintView->redraw();
	}
	else
	{
		// Send warning
	}
}

//------------------------------------------------------------------
// Get the color of the pixel in the original image at coord x and y
//------------------------------------------------------------------
GLubyte* ImpressionistDoc::GetOriginalPixel(int x, int y)
{
	if ( x < 0 ) 
		x = 0;
	else if ( x >= m_nWidth ) 
		x = m_nWidth-1;

	if ( y < 0 ) 
		y = 0;
	else if ( y >= m_nHeight ) 
		y = m_nHeight-1;

	return (GLubyte*)(m_ucBitmap + 3 * (y*m_nWidth + x));
}

//----------------------------------------------------------------
// Get the color of the pixel in the original image at point p
//----------------------------------------------------------------
GLubyte* ImpressionistDoc::GetOriginalPixel(const Point p)
{
	return GetOriginalPixel(p.x, p.y);
}

void ImpressionistDoc::setMousePos(Point source)
{
	m_pMousePos = source;
	m_pUI->m_origView->redraw();

}

Point ImpressionistDoc::getMousePos()
{
	return m_pMousePos;
}

void ImpressionistDoc::refresh()
{
	m_pUI->m_paintView->redraw();
	m_pUI->m_origView->redraw();
}

void ImpressionistDoc::edgeDetection()
{
	if (m_ucEdge) delete[] m_ucEdge;
	m_ucEdge = new unsigned char[m_nWidth * m_nHeight * 3];

	Convolution con = Convolution(m_ucBitmap, m_nWidth, m_nHeight);

	double threashold_sq = powf(m_pUI->getEdgeThreashold(), 2);

	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			double d_x = con.XGradient(j, i);
			double d_y = con.YGradient(j, i);

			if (powf(d_x, 2) + powf(d_y, 2) > threashold_sq)
			{
				m_ucEdge[(i * m_nWidth + j) * 3] = m_ucEdge[(i * m_nWidth + j) * 3 + 1] = m_ucEdge[(i * m_nWidth + j) * 3 + 2] = 255;
			}
			else
			{
				m_ucEdge[(i * m_nWidth + j) * 3] = m_ucEdge[(i * m_nWidth + j) * 3 + 1] = m_ucEdge[(i * m_nWidth + j) * 3 + 2] = 0;
			}
		}
	}
	
	m_ucOriginal = m_ucEdge;
	refresh();
}