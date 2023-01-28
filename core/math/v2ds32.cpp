/*
Author: github.com/annadostoevskaya
File: v2ds32.cpp
Date: 28/01/23 15:07:48

Description: <empty>
*/

class V2DS32
{
public:
    S32 x, y;

    V2DS32& operator+=(const V2DS32& a)
    {
        this->x += a.x;
        this->y += a.y;
        return *this;
    }

    friend V2DS32 operator+(V2DS32 lhs, const V2DS32& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    V2DS32& operator-=(const V2DS32& a)
    {
        this->x -= a.x;
        this->y -= a.y;
        return *this;
    }

    friend V2DS32 operator-(V2DS32 lhs, const V2DS32& rhs)
    {
        lhs -= rhs;
        return lhs;
    }   

    V2DS32& operator*=(const V2DS32& rhs)
    {
        this->x *= rhs.x;
        this->y *= rhs.y;
        return *this;
    }
    
    friend V2DS32 operator*(V2DS32 lhs, const V2DS32& rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    V2DS32& operator/=(const V2DS32& a)
    {
        this->x /= a.x;
        this->y /= a.y;
        return *this;
    }

    friend V2DS32 operator/(V2DS32 lhs, const V2DS32& rhs)
    {
        lhs /= rhs;
        return lhs;
    }
};

