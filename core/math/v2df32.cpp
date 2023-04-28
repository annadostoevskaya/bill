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

    V2DF32& operator+=(const V2DF32& rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }

    friend V2DF32 operator+(V2DF32 lhs, const V2DF32& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    V2DF32& operator-=(const V2DF32& rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;
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
   
    V2DF32& operator*=(const F32 rhs)
    {
        this->x *= rhs;
        this->y *= rhs;
        return *this;
    }

    friend V2DF32 operator*(V2DF32 lhs, const V2DF32& rhs)
    {
        lhs *= rhs;
        return lhs;
    }
    
    friend V2DF32 operator*(V2DF32 lhs, const F32 rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    friend V2DF32 operator*(const F32 rhs, V2DF32 lhs)
    {
        lhs *= rhs;
        return lhs;
    }

    V2DF32& operator/=(const V2DF32& rhs)
    {
        this->x /= rhs.x;
        this->y /= rhs.y;
        return *this;
    }

    V2DF32& operator/=(const F32 rhs)
    {
        this->x /= rhs;
        this->y /= rhs;
        return *this;
    }

    friend V2DF32 operator/(V2DF32 lhs, const V2DF32& rhs)
    {
        lhs /= rhs;
        return lhs;
    }

    friend V2DF32 operator/(V2DF32 lhs, const F32 rhs)
    {
        lhs /= rhs;
        return lhs;
    }

    friend V2DF32 operator/(const F32 rhs, V2DF32 lhs)
    {
        lhs /= rhs;
        return lhs;
    }

    inline F32 getLength()
    {
        return f32Sqrt(f32Square(this->x) + f32Square(this->y));
    }

    V2DF32 getNormalize()
    {
        F32 l = this->getLength();
        V2DF32 n;
        n.x = this->x / l;
        n.y = this->y / l;
        return n;
    }
    
    F32 inner(V2DF32 &a)
    {
        return this->x * a.x + this->y * a.y;
    }

    void tonorm()
    {
        F32 l = this->getLength();
        this->x /= l;
        this->y /= l;
    }
};

typedef V2DF32 P2DF32;

