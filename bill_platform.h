/* 
Author: github.com/annadostoevskaya
File: bill_platform.h
Date: September 25th 2022 9:51 pm 

Description: <empty>
*/

#ifndef BILL_PLATFORM_H
#define BILL_PLATFORM_H

#include "bill_math.h"

typedef struct GameMemory
{
    void *permanent_storage;
    size_t permanent_storage_size;
    
    void *persistent_storage;
    size_t persistent_storage_size;
} GameMemory;

typedef struct GameTime
{
    U64 start;
    U64 end;
    S64 dt;
} GameTime;

enum Button_State
{
    BUTTON_STATE_UP,
    BUTTON_STATE_DOWN,
    
    BUTTON_STATE_COUNT
};

enum Mouse_Button
{
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_MID,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_X1,
    MOUSE_BUTTON_X2,
    
    MOUSE_BUTTON_COUNT
};

typedef struct GameInputMouseButtonState
{
    Vec2S32 click_pos;
    union
    {
        Button_State enum_state;
        B32 b_state;
    };
} GameInputMouseButtonState;

typedef struct GameInputMouse
{
    GameInputMouseButtonState buttons_states[MOUSE_BUTTON_COUNT];
    Vec2S32 cursor_pos;
} GameInputMouse;

typedef struct GameInput
{
    GameInputMouse mouse;
} GameInput;


#endif //BILL_PLATFORM_H
