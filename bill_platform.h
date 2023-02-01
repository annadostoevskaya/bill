/* 
Author: github.com/annadostoevskaya
File: bill_platform.h
Date: September 25th 2022 9:51 pm 

Description: <empty>
*/

#ifndef BILL_PLATFORM_H
#define BILL_PLATFORM_H

#define PLATFORM_PERMANENT_STRG_SZ KB(4)
#define PLATFORM_PERSISTENT_STRG_SZ MB(256)

struct Tick
{
    U64 start;
    U64 end;
    S32 dt;
};

struct GameStorage
{
    void *permanent;
    S32 permanSize;
    void *persistent;
    S32 persistSize;
};

enum MouseBtn
{
    MOUSE_BTN_LEFT = 0,
    MOUSE_BTN_RIGHT,
    MOUSE_BTN_MIDDLE,

    MOUSE_BTN_COUNT,
    MOUSE_BTN_NULL
};

enum KeyboardBtn
{
    KEYB_BTN_RETURN = 0,
    KEYB_BTN_COUNT,
    KEYB_BTN_NULL
};

struct InputDevices
{
    S32 mouseX;
    S32 mouseY;
    B8 mouseBtns[MOUSE_BTN_COUNT];
    B8 keybBtns[KEYB_BTN_COUNT];
};

struct GameIO
{
    InputDevices *devices;
    RendererHandle *hRenderer;
    GameStorage *storage;
    Tick *tick;
};

#endif // BILL_PLATFORM_H

