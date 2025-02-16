//
// matrix.h
//
// The header file for matrix
//

#ifndef MATRIX_H
#define MATRIX_H

class Matrix
{
public:
    Matrix();
    Matrix(int size);
    Matrix(int m, int n, double value = 0.0);
    Matrix(int m, int n, double* values);

    ~Matrix();

    void    initialize(double value = 0.0);
    void    identity(int num);

    void    setValue(int x, int y, double num);
    double  getValue(int x, int y) const;

    int     getWidth() const;
    int     getHeight() const;

    double  InnerSum() const;

    static Matrix* String2Matrix(const char* string);
    static Matrix* GaussianBlurMatrix(int size, double sigma = 1.0);

private:
    int m, n;
    double** elements;
};

#endif