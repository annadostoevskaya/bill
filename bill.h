/* 
Author: github.com/annadostoevskaya
File: bill.h
Date: September 24th 2022 8:05 pm 

Description: <empty>
*/

#ifndef BILL_H
#define BILL_H

typedef struct GameState
{
    MemArena memory_arena;
    B16 initialize_flag;
    B16 debugPauseGame;
} GameState;

#endif //BILL_H
