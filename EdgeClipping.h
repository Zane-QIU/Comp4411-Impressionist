#ifndef EDGECLIPPING_H
#define EDGECLIPPING_H

namespace EdgeClipping
{
    void clip(unsigned char* bmp, int width, int height, int x, int y, int angle, int* size);
};

#endif