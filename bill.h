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

struct CueStick
{
    V2DS32 clipos;
    B8 click;
};

struct TableBoard
{
    P2DF32 p[4];
};

struct Table
{
    V2DS32 pos;
    S32 w;
    S32 h;
    TableBoard boards[6];
    Rect collider;
    ImageAsset img;
};

struct GameState
{
    M_Arena arena;
    // TODO(annad): Balls is Entities? Idk.
    // i mean struct Ball {...
    Entity balls[BALL_COUNT];
    Table table;

    CueStick cuestick;
    CollideEventQueue cequeue;

    F32 radius;
    B8 isInit;
};

#endif //BILL_H

