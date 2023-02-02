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
    
    V2DS32& operator=(const V2DS32& rhs)
    {
        this->x = rhs.x;
        this->x = rhs.y;
        return *this;
    }

    V2DS32& operator+=(const V2DS32& rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }

    friend V2DS32 operator+(V2DS32 lhs, const V2DS32& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    V2DS32& operator-=(const V2DS32& rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;
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
    
    V2DS32& operator*=(const S32 rhs)
    {
        this->x *= rhs;
        this->y *= rhs;
        return *this;
    }

    friend V2DS32 operator*(V2DS32 lhs, const V2DS32& rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    friend V2DS32 operator*(V2DS32 lhs, const S32 rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    V2DS32& operator/=(const V2DS32& rhs)
    {
        this->x /= rhs.x;
        this->y /= rhs.y;
        return *this;
    }

    friend V2DS32 operator/(V2DS32 lhs, const V2DS32& rhs)
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

    S32 inner(V2DS32 &a)
    {
        return this->x * a.x + this->y * a.y;
    }
};
