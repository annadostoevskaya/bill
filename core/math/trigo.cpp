/*
Author: github.com/annadostoevskaya
File: trigo.cpp
Date: 26/01/23 17:42:19

Description: https://www.computerenhance.com/p/turns-are-better-than-radians
*/

F32 turnCos(F32 x)
{
    return (F32)cos(TAU_F32 * x);
}

F32 turnSin(F32 x)
{
    return (F32)sin(TAU_F32 * x);
}

