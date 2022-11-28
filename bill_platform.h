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

enum Input_Button_States
{
    INPUT_BUTTON_STATE_UP = 0,
    INPUT_BUTTON_STATE_DOWN,
    
    INPUT_BUTTON_STATE_COUNT
};

enum Input_Mouse_Buttons
{
    INPUT_MOUSE_BUTTON_LEFT,
    INPUT_MOUSE_BUTTON_MID,
    INPUT_MOUSE_BUTTON_RIGHT,
    
    INPUT_MOUSE_BUTTON_COUNT
};

enum Input_Keyboard_Keys
{
    INPUT_KEYBOARD_KEYS_NULL = 0,
    // TODO(annad): ESC?
    INPUT_KEYBOARD_KEYS_RETURN,
    
    INPUT_KEYBOARD_KEYS_COUNT
};

typedef struct GameInputMouseButtonState
{
    Vec2Dim<S32> click_pos;
    union
    {
        Input_Button_States state;
        B32 b_state;
    };
} GameInputMouseButtonState;

typedef struct GameInputMouse
{
    GameInputMouseButtonState buttons_states[INPUT_MOUSE_BUTTON_COUNT];
    Vec2Dim<S32> cursor_pos;
} GameInputMouse;

typedef struct GameInputKeyboard
{
    Input_Button_States keys[INPUT_KEYBOARD_KEYS_COUNT];
} GameInputKeyboard;

typedef struct GameInput
{
    GameInputMouse mouse;
    GameInputKeyboard keyboard;
} GameInput;

inline void set_mouse_button_state(GameInputMouseButtonState *btn, Input_Button_States state, S32 x, S32 y)
{
    btn->state = state;
    btn->click_pos.x = x;
    btn->click_pos.y = y;
}


#endif // BILL_PLATFORM_H

