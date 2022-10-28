/* 
Author: github.com/annadostoevskaya
File: bill_math.h
Date: September 25th 2022 9:54 pm 

Description: <empty>
*/

#ifndef BILL_MATH_H
#define BILL_MATH_H

#include "base_types.h"
#include <math.h>

template <typename T>
struct Point2Dim
{
    T x, y;
};

template <typename T>
struct Vec2Dim
{
    T x, y;
    
    Vec2Dim<T>& operator+=(const Vec2Dim<T>& a)
    {
        this->x += a.x;
        this->y += a.y;
        return *this;
    }
    
    Vec2Dim<T>& operator-=(const Vec2Dim<T>& a)
    {
        this->x -= a.x;
        this->y -= a.y;
        return *this;
    }
    
    Vec2Dim<T>& operator*=(const T& a)
    {
        this->x *= a;
        this->y *= a;
        return *this;
    }
    
    Vec2Dim<T>& operator/=(const T& a)
    {
        this->x /= a;
        this->y /= a;
        return *this;
    }
    
    Vec2Dim<T>& operator-(const T& a)
    {
        this->x /= a;
        this->y /= a;
        return *this;
    }
    
    inline F64 getLength()
    {
        return (F64)sqrt((double)(this->y * this->y + this->x * this->x));
    }
    
    inline T innerProduct(const Vec2Dim<T>& a)
    {
        return this->x * a.x + this->y * a.y;
    }
};

template<typename T>
Vec2Dim<T> operator/(Vec2Dim<T> a, const T b)
{
    return (a /= b);
}

template<typename T>
Vec2Dim<T> operator*(Vec2Dim<T> a, const T b)
{
    return (a *= b);
}

template<typename T>
Vec2Dim<T> operator+(Vec2Dim<T> a, const Vec2Dim<T>& b)
{
    return (a += b);
}

template<typename T>
Vec2Dim<T> operator-(Vec2Dim<T> a, const Vec2Dim<T>& b)
{
    return (a -= b);
}

F32 square(F32 x)
{
    return x * x;
}

F32 turnCos(F32 x)
{
    return (F32)cos(TAU_F32 * x);
}

F32 turnSin(F32 x)
{
    return (F32)sin(TAU_F32 * x);
}

F32 defaultCos(F32 x)
{
    return (F32)cos(x);
}

F32 defaultSin(F32 x)
{
    return (F32)sin(x);
}

F32 defaultArcSin(F32 x)
{
    return (F32)(asin(x));
}

F32 defaultArcCos(F32 x)
{
    return (F32)(acos(x));
}

F32 defaultArcTan(F32 x)
{
    return (F32)atan(x);
}

union F32_S32
{
    F32 f32;
    S32 s32;
};

F32 f32Abs(F32 x)
{
    F32_S32 temp;
    temp.f32 = x;
    
    if(isLittleEndian())
    {
        temp.s32 = temp.s32 & 0x7fffffff;
    }
    else
    {
        temp.s32 = temp.s32 & 0xffffff7f;
    }
    
    return temp.f32;
}

S32 f32Round(F32 x)
{
    return lround(x);
}

#endif //BILL_MATH_H

