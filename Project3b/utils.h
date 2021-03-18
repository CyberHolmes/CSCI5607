#ifndef UTILS_H
#define UTILS_H

#define MIN_T 0.001
#define MAX_T 999999
#include <cstdint>

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

int log2_asm(int x){
	uint32_t y;
	asm ("\tbsr %1, %0\n"
		: "=r"(y)
		: "r" (x)
	);
	return y;
}

#endif