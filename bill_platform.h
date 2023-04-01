/* 
Author: github.com/annadostoevskaya
File: bill_platform.h
Date: September 25th 2022 9:51 pm 

Description: <empty>
*/

#ifndef BILL_PLATFORM_H
#define BILL_PLATFORM_H

#define PLATFORM_PERMANENT_STRG_SZ   KB(4)
#define PLATFORM_PERSISTENT_STRG_SZ  MB(64)
#define PLATFORM_ASSETS_STRG_SZ      MB(128)

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
    void *assets;
    S32 assetsSize;
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
    S32 wheelX;
    S32 wheelY;
    B8 mouseBtns[MOUSE_BTN_COUNT];
    B8 keybBtns[KEYB_BTN_COUNT];
};

struct Screen
{
    S32 w;
    S32 h;
    S32 pitch;
    U32 *buf;
};

struct GameIO
{
    InputDevices *devices;
    RendererHandle *hRenderer;
    Screen *screen;
    GameStorage *storage;
    Tick *tick;
};

#endif // BILL_PLATFORM_H

