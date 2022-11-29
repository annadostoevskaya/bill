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
    BALL_ENUM_WHITE = 0,
    BALL_ENUM_2,
    BALL_ENUM_3,
    BALL_ENUM_4,
    BALL_ENUM_5,
    BALL_ENUM_6,
    BALL_ENUM_7,
    BALL_ENUM_8,
    BALL_ENUM_9,
    BALL_ENUM_COUNT,
    
    BALL_ENUM_UNDEFINED
};

struct Ball
{
    S32 id;
    Vec2Dim<F32> vel;
    Vec2Dim<F32> pos;
};

struct GameState
{
    Arena memory_arena;
    Ball balls[BALL_ENUM_COUNT];
    Vec2Dim<F32> bill_cue;
    B16 initialize_flag;
};

#endif //BILL_H

