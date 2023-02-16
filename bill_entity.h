/*
Author: github.com/annadostoevskaya
File: bill_entity.h
Date: 16/02/23 13:43:10

Description: <empty>
*/

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

struct Entity
{
    EntityID id;

    B16 isInit;
    B16 isUpdated;
    F32 dtUpdate;

    V2DF32 p;
    V2DF32 v;
};

