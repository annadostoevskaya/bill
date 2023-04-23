/*
Author: github.com/annadostoevskaya
File: mmath.cpp
Date: 26/01/23 12:45:41

Description: <empty>
*/

#include <math.h>

inline F32 f32Square(F32 x)
{
    return x * x;
}

inline F32 f32Sqrt(F32 x)
{
    return sqrtf(x);
}

inline F32 f32Abs(F32 x)
{
    return fabsf(x);
}

inline F32 f32Round(F32 x)
{    
    return roundf(x);
}

inline B8 f32EpsCompare(F32 rhs, F32 lhs, F32 eps)
{
    return f32Abs(rhs - lhs) < eps;
}

inline F32 f32Floor(F32 x)
{
    return floorf(x);
}

inline F32 f32Ceil(F32 x)
{
    return ceilf(x);
}

inline F32 f32Infinity()
{
    union { F32 f; U32 u; } r;
    r.u = 0x7f800000;
    return r.f;
}

