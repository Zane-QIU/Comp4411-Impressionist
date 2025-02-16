//
// SharpBrush.h
//
// The header file for Blur Brush. 
//

#ifndef SHARPBRUSH_H
#define SHARPBRUSH_H

#include "ImpBrush.h"

class Convolution;
class Matrix;

class SharpBrush : public ImpBrush
{
public:
	SharpBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);

private:
	Convolution* convolution;
	Matrix* sharpKernel;
};

#endif