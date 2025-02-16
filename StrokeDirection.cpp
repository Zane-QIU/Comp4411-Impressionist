//
// StrokeDirection.cpp
//
// The implementation of obtaining stroke direction
//

#include "StrokeDirection.h"

#include "Convolution.h"

#include <math.h>

void drawRedLine(Point, Point, int);

StrokeDirection::StrokeDirection()
: m_pStartMousePos(0, 0), m_pEndMousePos(0, 0), m_pLastMousePos(-1, -1), m_nAngle(0)
{
}

// Start measuring stroke direction
void StrokeDirection::StrokeBegin(Point source, int offset)
{
    setStartMousePos(source);

    drawRedLine(m_pStartMousePos, m_pStartMousePos, offset);
}

// Update stroke direction
void StrokeDirection::StrokeMove(Point source, int offset)
{
    setEndMousePos(source);

    drawRedLine(m_pStartMousePos, m_pEndMousePos, offset);
}

// Stop measuring stroke direction, then calculate the angle
void StrokeDirection::StrokeEnd(Point source)
{
    setEndMousePos(source);

    m_nAngle = (int)(atan2f(m_pEndMousePos.y - m_pStartMousePos.y, m_pEndMousePos.x - m_pStartMousePos.x) * 180 / M_PI);

    if (m_nAngle < 0)
    {
        m_nAngle += 360;
    }
}

// ====================================================
//                      Accessors
// ====================================================

void StrokeDirection::setStartMousePos(Point source)
{
    m_pStartMousePos = source;
}

Point StrokeDirection::getStartMousePos()
{
    return m_pStartMousePos;
}

void StrokeDirection::setEndMousePos(Point source)
{
    m_pEndMousePos = source;
}

Point StrokeDirection::getEndMousePos()
{
    return m_pEndMousePos;
}

void StrokeDirection::setLastMousePos(Point source)
{
    m_pLastMousePos = source;
}

Point StrokeDirection::getLastMousePos()
{
    return m_pLastMousePos;
}

void StrokeDirection::setAngle(int angle)
{
    m_nAngle = angle;
}

int StrokeDirection::getAngle(ImpressionistDoc* pDoc, Point source, Point target, int type)
{
    if (type == STROKE_GRADIENT)
    {
        unsigned char* bmp = pDoc->m_ucBitmap;

        if (pDoc->m_pUI->getAnotherGradient())
        {
            if (pDoc->m_ucAnotherImage)
                bmp = pDoc->m_ucAnotherImage;
        }
        int width = pDoc->m_nPaintWidth;
        int height = pDoc->m_nPaintHeight;
        Convolution con = Convolution(bmp, width, height);

        int d_x = con.XGradient(source.x, source.y);
        int d_y = con.YGradient(source.x, source.y);

        int angle = (int)(atan2f(d_y, d_x) * 180 / M_PI) + 90;

        if (angle < 0)
        {
            angle += 360;
        }

        return angle;
    }
    else if (type == STROKE_BRUSH_DIRECTION)
    {
        int angle = 0;

        if (m_pLastMousePos.x == -1 && m_pLastMousePos.y == -1)
        {
            angle = -1;
        }
        else
        {
            angle = (int)(atan2f(source.y - m_pLastMousePos.y, source.x - m_pLastMousePos.x) * 180 / M_PI);

            if (angle < 0)
            {
                angle += 360;
            }

        }

        m_pLastMousePos = source;

        return angle;
    }

    return m_nAngle;
}

void StrokeDirection::resetLastMousePos()
{
    m_pLastMousePos = Point(-1, -1);
}

void drawRedLine(Point start, Point end, int offset)
{
    glColor3f(1.0, 0.0, 0.0);
    
    glLineWidth(1);
    glBegin(GL_LINES);
        glVertex2i(start.x, start.y + offset);
        glVertex2i(end.x, end.y + offset);
    glEnd();
}