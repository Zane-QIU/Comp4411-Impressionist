#include "ThreeDTree.h"

#include <iostream>
#include <stdio.h>
#include <string.h>

ThreeDTree::Color::Color()
: rgb(NULL), count(0)
{
}

ThreeDTree::Color::Color(int r, int g, int b)
: count(0)
{
    rgb = new unsigned char[3];
    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
}

ThreeDTree::Color::Color(unsigned char *color)
: count(0)
{
    rgb = new unsigned char[3];
    memcpy(rgb, color, 3);
}

ThreeDTree::Color::~Color()
{
    delete[] rgb;
}

int ThreeDTree::Color::r() const
{
    return int(rgb[0]);
}

int ThreeDTree::Color::g() const
{
    return int(rgb[1]);
}

int ThreeDTree::Color::b() const
{
    return int(rgb[2]);
}

int ThreeDTree::Color::getCount() const
{
    return count;
}

int ThreeDTree::Color::dist(Color *another)
{
    return pow((r() - another->r()), 2) + pow((g() - another->g()), 2) + pow((b() - another->b()), 2);
}

void ThreeDTree::Color::add()
{
    count++;
}

ThreeDTree::ThreeDTree(Color **colors, int size, RGB rgb)
    : left(NULL), right(NULL), dim(rgb)
{
    if (size > 1)
        sortColor(colors, size, rgb);

    color = colors[size / 2];

    if (size == 1)
        return;

    int size_l = int(size / 2);
    int size_r = size - int(size / 2) - 1;

    RGB next_dim = RGB((rgb + 1) % 3);
    if (size_l > 0)
    {
        Color **l = colors;
        left = new ThreeDTree(l, size_l, next_dim);
    }

    if (size_r > 0)
    {
        Color **r = colors + size_l + 1;
        right = new ThreeDTree(r, size_r, next_dim);
    }
}

ThreeDTree::Color *ThreeDTree::SearchNearest(Color *target)
{
    return search(this, target)->color;
}

int ThreeDTree::dist(ThreeDTree *another)
{
    return color->dist(another->color);
}

void ThreeDTree::sortColor(Color **colors, int size, RGB rgb)
{
    if (size == 1)
        return;

    int size_l, size_r;
    Color **l = NULL, **r = NULL;

    split(colors, size, size_l, size_r, l, r);

    sortColor(l, size_l, rgb);
    sortColor(r, size_r, rgb);

    merge(colors, size_l, size_r, l, r, rgb);

    delete[] l;
    delete[] r;
}

void ThreeDTree::split(Color **colors, int size, int &size_l, int &size_r, Color **&l, Color **&r)
{
    size_l = floor(double(size) / 2);
    size_r = ceil(double(size) / 2);

    l = new Color *[size_l];
    for (int i = 0; i < size_l; i++)
    {
        l[i] = colors[i];
    }

    r = new Color *[size_r];
    for (int i = 0; i < size_r; i++)
    {
        r[i] = colors[size_l + i];
    }
}

void ThreeDTree::merge(Color **&colors, int size_l, int size_r, Color **l, Color **r, RGB rgb)
{
    int a = 0, b = 0;

    while (a + b < size_l + size_r)
    {
        bool larger = false;
        if (a < size_l && b < size_r)
        {
            switch (rgb)
            {
            case R:
                if (l[a]->r() > r[b]->r())
                    larger = true;
                break;
            case G:
                if (l[a]->g() > r[b]->g())
                    larger = true;
                break;
            case B:
                if (l[a]->b() > r[b]->b())
                    larger = true;
                break;
            default:
                break;
            }
        }
        else if (b < size_r)
        {
            larger = true;
        }

        if (larger)
        {
            colors[a + b] = r[b];
            b++;
        }
        else
        {
            colors[a + b] = l[a];
            a++;
        }
    }
}

ThreeDTree *ThreeDTree::search(ThreeDTree *root, Color *target)
{
    if (root == NULL)
        return NULL;

    ThreeDTree *nextBranch = NULL, *otherBranch = NULL;

    switch (root->dim)
    {
    case R:
        if (target->r() < root->color->r())
        {
            nextBranch = root->left;
            otherBranch = root->right;
        }
        else
        {
            nextBranch = root->right;
            otherBranch = root->left;
        }
        break;
    case G:
        if (target->g() < root->color->g())
        {
            nextBranch = root->left;
            otherBranch = root->right;
        }
        else
        {
            nextBranch = root->right;
            otherBranch = root->left;
        }
        break;
    case B:
        if (target->b() < root->color->b())
        {
            nextBranch = root->left;
            otherBranch = root->right;
        }
        else
        {
            nextBranch = root->right;
            otherBranch = root->left;
        }
        break;
    }

    ThreeDTree *temp = search(nextBranch, target);
    ThreeDTree *best = NULL;
    if (temp != NULL && temp->color->dist(target) < root->color->dist(target))
    {
        best = temp;
    }
    else
    {
        best = root;
    }

    int radiusSquared = best->color->dist(target);
    int distance;
    switch (root->dim)
    {
    case R:
        distance = target->r() - root->color->r();
        break;
    case G:
        distance = target->g() - root->color->g();
        break;
    case B:
        distance = target->b() - root->color->b();
        break;
    }

    if (radiusSquared >= distance * distance)
    {
        temp = search(otherBranch, target);

        if (temp != NULL && temp->color->dist(target) < best->color->dist(target))
        {
            best = temp;
        }
        else
        {
            best = best;
        }
    }

    return best;
}