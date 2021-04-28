#ifndef MATRIX_H
#define MATRIX_H
#include "vec3.h"

class Matrix{
    const int nr = 4;//num rows
    const int nl = 4;//num cols
    float m[4][4];
public:
    Matrix(){float m[4][4]{};}    
    ~Matrix(){}
    float& GetElement(int i, int j);
    Matrix operator*(Matrix m2);
    vec3 operator*(vec3 v); 
    Matrix operator*(float f); 
    Matrix operator+(Matrix m2);
    Matrix operator-(Matrix m2);
    Matrix Transpose();
    Matrix Inverse();       
};

#endif