#ifndef PAINTLY_H
#define PAINTLY_H

#include <FL/gl.h>

#include "ImpBrush.h"

class ImpressionistDoc;

class Pointll
{
public:
	Pointll(int x, int y);
	~Pointll();
	Point xy;
	Pointll* next;
};

class Paintly
{
public:
    Paintly(ImpressionistDoc* pDoc);

    ~Paintly();

    unsigned char* m_ucRefImage;

    ImpressionistDoc* m_pDoc;
    
    int width, height;

    int m_nNumMax;

    Pointll* m_pllMax;

    void genRefImage(unsigned char* bmp, int width, int height, int size, double sigma = 1.0);


    static double ColorDiff(GLubyte* color1, GLubyte* color2);

    void DrawPaintly();

	void paintLayer(unsigned char* canvas, unsigned char* m_ucRefImage, int R, int layer, int time);

	double AreaError(int x, int y, double grid, Point& max, double* D);
		
    GLubyte* GetColor(unsigned char* source, int x, int y);
	
    void makeSplineStroke(int x_0, int y_0, int R, unsigned char* m_ucRefImage);

};

#endif