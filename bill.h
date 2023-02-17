/* 
Author: github.com/annadostoevskaya
File: bill.h
Date: September 24th 2022 8:05 pm 

Description: <empty>
*/

#ifndef BILL_H
#define BILL_H

#include "bill_entity.h"
#include "bill_colevent.h"

struct Rect
{
    S32 x;
    S32 y;
    S32 w;
    S32 h;
};

struct LNP2DS32
{
    P2DS32 a;
    P2DS32 b;
};

struct CueStick
{
    V2DS32 clipos;
    B8 click;
};

struct GameState
{
    M_Arena arena;
    // TODO(annad): Balls is Entities? Idk.
    Entity balls[BALL_COUNT];
    CueStick cuestick;
    
    Rect table;
    LNP2DS32 baulkline;

    CollideEventQueue cequeue;
    
    S32 base;
    F32 balldiam;

    B8 isInit;
};

#endif //BILL_H

