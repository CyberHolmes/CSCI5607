#ifndef UTILS_H
#define UTILS_H

#define MIN_T 0.001
#define MAX_T 999999
#include <cstdint>
#include <cmath>
#include <stdlib.h>

// #define PATH_USELIGHT
#define rand01 ((double) rand() / (RAND_MAX))  //random number between 0 and 1
#define randn11 (2*rand01-1)  //random number from -1 to 1
#define randn1 (rand01-1)  //random number from -0.5 to 0.5

inline float mymin(float a, float b, float c){
    float out = MAX_T;
    out = (out>a)? a:out;
    out = (out>b)? b:out;
    out = (out>c)? c:out;
    return out;
}

inline float mymax(float a, float b, float c){
    float out = -MAX_T;
    out = (out<a)? a:out;
    out = (out<b)? b:out;
    out = (out<c)? c:out;
    return out;
}
#ifdef _MSC_VER
inline int log2_asm(int x) {
    return log2(x);
}
#else
inline int log2_asm(int x){
	uint32_t y(0);
	asm ("\tbsr %1, %0\n"
		: "=r"(y)
		: "r" (x)
	);
	return y;
}
#endif
#endif
