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

#pragma pack(push, 1)
struct BMPHeader
{
    U16 type; // 2
    U32 size; // 6
    U32 RESERVED; // 10
    U32 offset; // 14 bytes
};

struct BMPInfo
{
    U32 size;
    S32 width;
    S32 height;
    U16 planes; // STUB
    U16 bitcount;
    U32 compression; // STUB
    U32 imgsize;
    S32 xpxperMeter; // STUB
    S32 ypxperMeter; // STUB
    U32 colorsUsed; // STUB
    U32 colorsImportant; // STUB // 40 
    U32 BGRA[1]; // STUB // 44 bytes
};
#pragma pack(pop)

struct BMP 
{
    BMPHeader   *header;
    BMPInfo     *info;
    U32         *bitmap;
};

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

struct Table
{
    V2DS32 pos;
    Rect collider;
    BMP img;
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
    
    S32 base;
    F32 balldiam;

    B8 isInit;
};

#endif //BILL_H

