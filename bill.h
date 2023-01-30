/* 
Author: github.com/annadostoevskaya
File: bill.h
Date: September 24th 2022 8:05 pm 

Description: <empty>
*/

#ifndef BILL_H
#define BILL_H

#define BALL_RADIUS 15

enum Ball
{
    BALL_WHITE = 0,
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

struct CollideInfo
{
    S32 ball_a_idx;
    S32 ball_b_idx;
    F32 dt;
};

#define PQ_MAX_ITEMS_COUNT 16

struct PriorityQueue
{
    CollideInfo items[PQ_MAX_ITEMS_COUNT];
    S32 size;
    S32 cursor;
};

struct Cue
{
    V2DS32 startPos;
    V2DS32 endPos;
    V2DF32 impactPower;
    B8 startFlag;
};

struct Entity
{
    V2DS32 p;
    V2DS32 v;
    B8 isInit;
};

struct GameState
{
    M_Arena arena;
    Entity balls[BALL_COUNT];

    Cue cue;
    Vec2Dim<F32> bill_cue;
    PriorityQueue pq;

    B8 isInit;
};

#endif //BILL_H

