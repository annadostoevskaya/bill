/*
Author: github.com/annadostoevskaya
File: mmath.cpp
Date: 26/01/23 12:45:41

Description: <empty>
*/

#include <math.h>

F32 f32Square(F32 x)
{
    return x * x;
}

F32 f32Sqrt(F32 x)
{
    return sqrtf(x);
}

F32 f32Abs(F32 x)
{
    return fabsf(x);
}

F32 f32Round(F32 x)
{    
    return roundf(x);
}

B8 f32EpsCompare(F32 rhs, F32 lhs, F32 eps)
{
    return f32Abs(rhs - lhs) < eps;
}

F32 f32Infinity()
{
    union { F32 f; U32 u; } r;
    r.u = 0x7f800000;
    return r.f;
}

