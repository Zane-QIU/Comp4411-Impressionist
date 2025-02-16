//
// CircleBrush.cpp
//
// The implementation of Circle Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include "ImpressionistDoc.h"
#include "ImpressionistUI.h"
#include "CircleBrush.h"

#include <math.h>

extern float frand();

CircleBrush::CircleBrush( ImpressionistDoc* pDoc, char* name ) :
	ImpBrush(pDoc,name)
{
}

void CircleBrush::BrushBegin( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	BrushMove( source, target );
}

void CircleBrush::BrushMove( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	if ( pDoc == NULL ) {
		printf( "CircleBrush::BrushMove  document is NULL\n" );
		return;
	}

    int size = pDoc->getSize();
	int r = size / 2;
    int triangle_amount = 60;
    double step = M_PI * 2 / triangle_amount;

	glBegin(GL_TRIANGLE_FAN);
		SetColor( source );

		glVertex2d(target.x, target.y);
        for (int i = 0; i <= triangle_amount; i++) {
            glVertex2d(target.x + r * cos(i * step), target.y + r * sin(i * step));
        }
	glEnd();
}

void CircleBrush::BrushEnd( const Point source, const Point target )
{
	// do nothing so far
}

