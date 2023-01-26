/* 
Author: github.com/annadostoevskaya
File: bill_math.h
Date: September 25th 2022 9:54 pm 

Description: <empty>
*/

#ifndef BILL_MATH_H
#define BILL_MATH_H
#define PRINT_VEC(V) printf("%s.x: %f\n%s.y: %f\n", #V, V.x, #V, V.y)

struct Point2Dim
{
    F32 x;
    F32 y;
};

template<typename T>
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

F32 defaultCos(F32 x)
{
    return (F32)cos(x);
}

F32 defaultSin(F32 x)
{
    return (F32)sin(x);
}

F32 defaultCsc(F32 x)
{
    return 1.0f / (F32)sin(x);
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

#endif //BILL_MATH_H

