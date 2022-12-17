/* 
Author: github.com/annadostoevskaya
File: bill.h
Date: September 24th 2022 8:05 pm 

Description: <empty>
*/

#ifndef BILL_H
#define BILL_H

enum Ball_Enum 
{
    BALL_WHITE = 0,
    BALL_1 = 0,
    BALL_2,
    BALL_3,
    BALL_4,
    BALL_5,
    BALL_6,
    BALL_7,
    BALL_8,
    BALL_9,
    BALL_COUNT,
    
    BALL_ENUM_UNDEFINED
};

struct Ball
{
    S32 id;
    Vec2Dim<F32> vel;
    Vec2Dim<F32> pos;
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


struct GameState
{
    Arena memory_arena;
    Ball balls[BALL_COUNT];
    Vec2Dim<F32> bill_cue;
    B16 initialize_flag;
    
    PriorityQueue pq;
};

#endif //BILL_H

