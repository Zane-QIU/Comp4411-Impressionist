//
// PointBrush.cpp
//
// The implementation of Line Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include "ImpressionistDoc.h"
#include "ImpressionistUI.h"
#include "ScatteredCircleBrush.h"

#include <math.h>

extern float frand();

ScatteredCircleBrush::ScatteredCircleBrush( ImpressionistDoc* pDoc, char* name ) :
	ImpBrush(pDoc,name)
{
}

void ScatteredCircleBrush::BrushBegin( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	BrushMove( source, target );
}

void ScatteredCircleBrush::BrushMove( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	if ( pDoc == NULL ) {
		printf( "ScatteredCircleBrush::BrushMove document is NULL\n" );
		return;
	}

    int size = pDoc->getSize();
	int r = size / 2;
    int triangle_amount = 60;
    double step = M_PI * 2 / triangle_amount;

	int num = (int)(frand() * 2 + 3);

	for (int i = 0; i < num; i++)
	{
		double x_offset = frand() * size - ceil(size / 2);
		double y_offset = frand() * size - ceil(size / 2);

		glBegin(GL_TRIANGLE_FAN);
			SetColor(Point(source.x + x_offset, source.y + y_offset));

			glVertex2d(target.x + x_offset, target.y + y_offset);
        	for (int i = 0; i <= triangle_amount; i++) {
        	    glVertex2d(target.x + r * cos(i * step) + x_offset, target.y + r * sin(i * step) + y_offset);
        	}
		glEnd();
	}
}

void ScatteredCircleBrush::BrushEnd( const Point source, const Point target )
{
	// do nothing so far
}

