//
// PointBrush.cpp
//
// The implementation of Line Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include "ImpressionistDoc.h"
#include "ImpressionistUI.h"
#include "ScatteredPointBrush.h"

extern float frand();

ScatteredPointBrush::ScatteredPointBrush(ImpressionistDoc* pDoc, char* name) :
	ImpBrush(pDoc,name)
{
}

void ScatteredPointBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	BrushMove(source, target);
}

void ScatteredPointBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("ScatteredPointBrush::BrushMove document is NULL\n");
		return;
	}

	int size = pDoc->getSize();
	Point new_source = Point(source.x - size / 2, source.y + size / 2);
	Point new_target = Point(target.x - size / 2, target.y + size / 2);

	glPointSize(1.0f);

	glBegin(GL_POINTS);
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				if (frand() < 0.15)
				{
					SetColor(new_source);
					glVertex2d(new_target.x + j, new_target.y - i);
				}
				new_source.x += 1;
			}
			new_source.y -= 1;
			new_source.x = source.x - size / 2;
		}
	glEnd();
}

void ScatteredPointBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}

