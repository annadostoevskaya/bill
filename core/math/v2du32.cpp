/*
Author: github.com/annadostoevskaya
File: core\math\v2du32.cpp
Date: 04/04/23 23:46:06

Description: <empty>
*/

class V2DU32
{
public:
    U32 x, y;
    
    V2DU32& operator+=(const V2DU32& rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }

    friend V2DU32 operator+(V2DU32 lhs, const V2DU32& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    V2DU32& operator-=(const V2DU32& rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;
        return *this;
    }

    friend V2DU32 operator-(V2DU32 lhs, const V2DU32& rhs)
    {
        lhs -= rhs;
        return lhs;
    }   

    V2DU32& operator*=(const V2DU32& rhs)
    {
        this->x *= rhs.x;
        this->y *= rhs.y;
        return *this;
    }
    
    V2DU32& operator*=(const U32 rhs)
    {
        this->x *= rhs;
        this->y *= rhs;
        return *this;
    }

    friend V2DU32 operator*(V2DU32 lhs, const V2DU32& rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    friend V2DU32 operator*(V2DU32 lhs, const U32 rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    V2DU32& operator/=(const V2DU32& rhs)
    {
        this->x /= rhs.x;
        this->y /= rhs.y;
        return *this;
    }

    friend V2DU32 operator/(V2DU32 lhs, const V2DU32& rhs)
    {
        lhs /= rhs;
        return lhs;
    }

    inline F32 getLength()
    {
        return f32Sqrt(f32Square((F32)this->x) + f32Square((F32)this->y));
    }

    V2DF32 getNormalize()
    {
        F32 l = this->getLength();
        V2DF32 n;
        n.x = this->x / l;
        n.y = this->y / l;
        return n;
    }

    U32 inner(V2DU32 &a)
    {
        return this->x * a.x + this->y * a.y;
    }
};

typedef V2DU32 P2DU32;

