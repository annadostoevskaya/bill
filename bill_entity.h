/*
Author: github.com/annadostoevskaya
File: bill_entity.h
Date: 16/02/23 13:43:10

Description: <empty>
*/

enum EntityID
{
    CUE_BALL = 0,
    BALL_1,
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
    BALL_COUNT,
    
    BALL_UNDEFINED
};

struct Entity
{
    EntityID id; // 4 

    B16 isInit; 
    B16 isUpdated; // 8
    F32 dtUpdate; // 12 

    V2DF32 p; // 20
    V2DF32 v; // 28

    HImage img; // +8+4
                    // 28+12 = 40
};



