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
    Ball_Enum id;
    Vec2Dim<F32> vel;
    Vec2Dim<F32> pos;
};

struct GameState
{
    MemArena memory_arena;
    Ball ball[BALL_ENUM_COUNT];
    
    B16 initialize_flag;
    B16 DEBUG_pause_game;
};

#endif //BILL_H
