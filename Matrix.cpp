//
// Matrix.cpp
//
// The implementation of Matrix. Matrix computation for convolution and other matrix operation
//

#include "Matrix.h"

#include <iostream>
#include <string>
#include <cstdio>

#ifndef M_PI
#define M_PI 3.1415926535F
#endif

Matrix::Matrix()
: elements(nullptr), m(0), n(0)
{
}

Matrix::Matrix(int size)
: m(size), n(size)
{
    elements = new double*[size];
    for (int i = 0; i < size; i++)
    {
        elements[i] = new double[size];
    }

    initialize();
}

Matrix::Matrix(int m, int n, double value)
: m(m), n(n)
{
    elements = new double*[m];
    for (int i = 0; i < m; i++)
    {
        elements[i] = new double[n];
    }

    initialize(value);
}

Matrix::Matrix(int m, int n, double* values)
: m(m), n(n)
{
    elements = new double*[m];
    for (int i = 0; i < m; i++)
    {
        elements[i] = new double[n];
        memcpy(elements[i], values + i * n, n * sizeof(double));
    }
}

Matrix::~Matrix()
{
    for (int i = 0; i < m; i++)
    {
        delete[] elements[i];
    }

    delete[] elements;
}

void Matrix::initialize(double value)
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            elements[i][j] = value;
        }
    }
}

void Matrix::identity(int num = 1)
{
    if (m != n) return;
    
    initialize();
    for (int i = 0; i < m; i++)
    {
        elements[i][i] = num;
    }
}

void Matrix::setValue(int x, int y, double num)
{
    elements[x][y] = num;
}

double Matrix::getValue(int x, int y) const
{
    return elements[x][y];
}

int Matrix::getWidth() const
{
    return n;
}

int Matrix::getHeight() const
{
    return m;
}

double Matrix::InnerSum() const
{
    double sum = 0;

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            sum += elements[i][j];
        }
    }

    return sum;
}

Matrix* Matrix::String2Matrix(const char* string)
{
    int length = strlen(string);
    char* str = new char[length + 1];
    double* values = new double[length + 1];

    strcpy(str, string);
    str[length] = '\0';

    int row = 0;
    int col = 0;
    int counter = 0;

    int temp_col = 0;

    char* token = strtok(str, ",");
    while (token != NULL)
    {
        if (row != 0)
        {
            if (token[0] == '\n')
            {
                if (temp_col + 1 != col)
                {
                    delete[] str;
                    delete[] values;

                    return NULL;
                }
                else
                {
                    row++;
                    temp_col = 0;
                }
            }
            else
            {
                temp_col++;
            }
        }
        else
        {
            if (token[0] == '\n')
            {
                row++;
            }
            else
            {
                col++;
            }
        }

        if (token[0] == '\n')
        {
            values[counter] = std::stod(token + 1);
        }
        else
        {
            values[counter] = std::stod(token);
        }

        token = strtok(NULL, ",");
        counter++;
    }

    if (temp_col + 1 != col) return NULL;

    row++;

    Matrix* result = new Matrix(row, col, values);
    delete[] values;
    delete[] str;

    return result;
}

Matrix* Matrix::GaussianBlurMatrix(int size, double sigma)
{
    Matrix* result = new Matrix(size * 2 + 1);

    double r, s = 2.0 * sigma * sigma;

    for (int x = -size; x <= size; x++)
    {
        for (int y  = -size; y <= size; y++)
        {
            r = sqrt(x * x + y * y);
            double value = (exp(-(r * r) / s)) / (M_PI * s);
            result->setValue(x + size, y + size, value);
        }
    }

    return result;
}