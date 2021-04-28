#include "matrix.h"

float& Matrix::GetElement(int i, int j){
    return m[i][j];
};
Matrix Matrix::operator*(Matrix m2){
    Matrix newm = Matrix();
    for (int i=0;i<4;i++){
        for (int j=0;j<4;j++){
            float s = 0;
            for (int j1=0;j1<4;j1++){
                for (int i2=0;i2<4;i2++){
                    s += m[i][j1] * m2.GetElement(i2,j);
                }
            }
            newm.GetElement(i,j) = s;
        }
    }
    return newm;
};
vec3 Matrix::operator*(vec3 v){
    float s[4] {};
    for (int i=0;i<4;i++){
        s[i] = v.x * m[i][0] + v.y * m[i][1] + v.z * m[i][2] + m[i][3];
    }
    return (s[3]==0)? vec3(s[0],s[1],s[2]) : vec3(s[0]/s[3],s[1]/s[3],s[2]/s[3]);
};  
Matrix Matrix::operator*(float f){
    Matrix newm = Matrix();
    for (int i=0;i<4;i++){
        for (int j=0;j<4;j++){
            newm.GetElement(i,j) = m[i][j] * f;
        }
    }
    return newm;
}; 
Matrix Matrix::operator+(Matrix m2){
    Matrix newm = Matrix();
    for (int i=0;i<4;i++){
        for (int j=0;j<4;j++){
            newm.GetElement(i,j) = m[i][j] + m2.GetElement(i,j);
        }
    }
    return newm;
};
Matrix Matrix::operator-(Matrix m2){
    Matrix newm = Matrix();
    for (int i=0;i<4;i++){
        for (int j=0;j<4;j++){
            newm.GetElement(i,j) = m[i][j] - m2.GetElement(i,j);
        }
    }
    return newm;
};
Matrix Matrix::Transpose(){
    Matrix newm = Matrix();
    for (int i=0;i<4;i++){
        for (int j=0;j<4;j++){
            newm.GetElement(i,j) = m[j][i];
        }
    }
    return newm;
};
Matrix Matrix::Inverse(){
    return Matrix();
};
 