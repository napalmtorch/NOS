#pragma once
#include <lib/stdint.h>

/// @brief Get absolute integer value @param i Intergral value @return Absolute of integral value
static inline int abs(int i) { return i < 0 ? -i : i; }

/// @brief Limit integer value within range @param i Value to clamp @param min Minimum value @param max Maximum value @return Clamped value
static inline int clamp(int i, int min, int max)
{
    if (i < min) { i = min; }
    if (i > max) { i = max; }
    return i;
}

double acos(double x);

double asin(double x);

double atan(double x);

double atan2(double x, double y);

double cos(double x);

double cosh(double x);

double sin(double x);

double sinh(double x);

double tanh(double x);

double exp(double x);

double frexp(double x, int* exponent);

double ldexp(double x, int exponent);

double log(double x);

double log10(double x);

double modf(double x, double* integer);

double pow(double x, double y);

/// @brief Calculate square root of double floating-point value @param x Value @return Square root of value
double sqrt(double x);

/// @brief Round value up @param x Value to round @return Rounded value
double ceil(double x);

/// @bried Get absolute double floating-point value @param Integral value @return Absolute of integral value
double fabs(double x);

/// @brief Round value down @param x Value to round @return Rounded value
double floor(double x);

double fmod(double x, double y);