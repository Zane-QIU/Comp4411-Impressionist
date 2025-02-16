// BlurBrush.h
//
// The header file for Blur Brush. 

#ifndef BLURBRUSH_H
#define BLURBRUSH_H

#include "ImpBrush.h"

class Convolution;
class Matrix;

class BlurBrush : public ImpBrush
{
public:
	BlurBrush(ImpressionistDoc* pDoc = NULL, char* name = nullptr);

	void BrushBegin(const Point source, const Point target) override;
	void BrushMove(const Point source, const Point target) override;
	void BrushEnd(const Point source, const Point target) override;
	char* BrushName(void);

private:
	Convolution* convFilter;
	Matrix* blurKernel;
};

#endif