//
// strokedirection.h
//
// The header file for getting stroke direction
//

#ifndef STROKEDIRECTION_H
#define STROKEDIRECTION_H

#include "ImpressionistDoc.h"
#include "ImpressionistUI.h"

class Point;

class ImpressionistDoc;

class StrokeDirection
{
public:
    StrokeDirection();

    void    StrokeBegin(Point source, int offset);
    void    StrokeMove(Point source, int offset);
    void    StrokeEnd(Point source);

    void    setStartMousePos(Point source);
    Point   getStartMousePos();

    void    setEndMousePos(Point source);
    Point   getEndMousePos();

    void    setLastMousePos(Point source);
    Point   getLastMousePos();

    void    setAngle(int angle);
    int     getAngle(ImpressionistDoc* bmp, Point source, Point target, int type);

    void    resetLastMousePos();

private:
    Point m_pStartMousePos;
    Point m_pEndMousePos;
    Point m_pLastMousePos;

    int m_nAngle;
};

#endif