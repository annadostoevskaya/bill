/*
Author: github.com/annadostoevskaya
File: v2df32.cpp
Date: 26/01/23 12:17:05

Description: <empty>
*/

class V2DF32
{
public:
    F32 x, y;

    V2DF32& operator+=(const V2DF32& a)
    {
        this->x += a.x;
        this->y += a.y;
        return *this;
    }

    friend V2DF32 operator+(V2DF32 lhs, const V2DF32& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    V2DF32& operator-=(const V2DF32& a)
    {
        this->x -= a.x;
        this->y -= a.y;
        return *this;
    }

    friend V2DF32 operator-(V2DF32 lhs, const V2DF32& rhs)
    {
        lhs -= rhs;
        return lhs;
    }   

    V2DF32& operator*=(const V2DF32& rhs)
    {
        this->x *= rhs.x;
        this->y *= rhs.y;
        return *this;
    }
    
    friend V2DF32 operator*(V2DF32 lhs, const V2DF32& rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    V2DF32& operator/=(const V2DF32& a)
    {
        this->x /= a.x;
        this->y /= a.y;
        return *this;
    }

    friend V2DF32 operator/(V2DF32 lhs, const V2DF32& rhs)
    {
        lhs /= rhs;
        return lhs;
    }
};

