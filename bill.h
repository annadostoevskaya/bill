/* 
Author: github.com/annadostoevskaya
File: bill.h
Date: September 24th 2022 8:05 pm 

Description: <empty>
*/

#ifndef BILL_H
#define BILL_H

#define BALL_RADIUS 15.0f

enum EntityID
{
    CUE_BALL = 0,
    BALL_BLACK = 1,

    BALL_1 = 0,
    BALL_2,
    BALL_3,
    BALL_4,
    BALL_5,
    BALL_6,
    BALL_7,
    BALL_8,
    BALL_9,
    BALL_10,
    BALL_11,
    BALL_12,
    BALL_13,
    BALL_14,
    BALL_15,
    BALL_16,
    BALL_COUNT,
    
    BALL_UNDEFINED
};

enum CollideType
{
    COLLIDE_WALL,
    COLLIDE_ONE_BALL,
    COLLIDE_TWO_BALL,

    COLLIDE_COUNT,
    COLLIDE_UNDEFINED
};

struct CollideEvent
{
    EntityID eid;
    F32 dtBefore;
    CollideType type;

    void *ctx;
};

struct BallsCollide
{
    EntityID idxBallA;
    EntityID idxBallB;
    F32 timeBefore;
};

#define COLLIDE_EVENT_QUEUE_COUNT 32

struct CollideEventQueue
{
    CollideEvent *items;
    S32 size;
    S32 cursor;
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

struct Entity
{
    EntityID id;

    B16 isInit;
    B16 isUpdated;
    F32 dtUpdate;

    V2DF32 p;
    V2DF32 v;
};

struct GameState
{
    M_Arena arena;
    // TODO(annad): Balls is Entities? Idk.
    Entity balls[BALL_COUNT];
    CueStick cuestick;
    Rect table;

    CollideEventQueue cequeue;

    B8 isInit;
};

#endif //BILL_H

