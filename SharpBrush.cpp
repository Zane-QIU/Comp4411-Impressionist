//
// SharpBrush.cpp
//
// The implementation of Sharp Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include "ImpressionistDoc.h"
#include "ImpressionistUI.h"
#include "SharpBrush.h"

#include "Matrix.h"
#include "Convolution.h"

extern float frand();

SharpBrush::SharpBrush(ImpressionistDoc* pDoc, char* name)
: ImpBrush(pDoc,name), convolution(nullptr), sharpKernel(nullptr)
{
}

void SharpBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	int size = pDoc->getSize();

	glPointSize((float)size);

	BrushMove(source, target);
}

void SharpBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();

	ImpressionistUI* dlg=pDoc->m_pUI;

	if (pDoc == nullptr) {
		printf("SharpBrush::BrushMove document is NULL\n");
		return;
	}

	if (convolution == nullptr)
		convolution = new Convolution(pDoc->m_ucBitmap, pDoc->m_nWidth, pDoc->m_nHeight);

	if (sharpKernel == nullptr)
	{
		int k_size = pDoc->getBlurSharpLevel() * 2 + 1;
		sharpKernel = new Matrix(k_size, k_size, -1);
		sharpKernel->setValue(pDoc->getBlurSharpLevel(), pDoc->getBlurSharpLevel(), -sharpKernel->InnerSum());
	}

	double color[3] = {0.0, 0.0, 0.0};

	glBegin(GL_POINTS);

		convolution->ConvolutionFilter(*sharpKernel, source.x, source.y, false, false, color);
		glColor3d(color[0] / 255, color[1] / 255, color[2] / 255);

		glVertex2d( target.x, target.y );

	glEnd();
}

void SharpBrush::BrushEnd( const Point source, const Point target )
{
	delete convolution;
	convolution = nullptr;
	delete sharpKernel;
	sharpKernel = nullptr;
}

