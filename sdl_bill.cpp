/* 
Author: github.com/annadostoevskaya
File: sdl_bill.cpp
Date: September 19th 2022 10:19 pm 

Description: <empty>
*/

#include "core/base.h"
#include <SDL2/SDL.h>

#if _OS_WINDOWS
# include <Windows.h>
#endif
#include <stdarg.h>
#include "bill_renderer.h"
#include "bill_platform.h"

// NOTE(annad): On anon structures
#pragma warning(disable : 4201)

typedef struct RGBA_U8
{
    union
    {
        struct
        {
            U8 r;
            U8 g;
            U8 b;
            U8 a;
        };
        
        U8 e[4];
    };
} RGBA_U8;

#define BILL_CFG_FPS            30
#define BILL_CFG_WINDOW_TITLE   "bill"
#define BILL_CFG_HEIGHT         540
#define BILL_CFG_WIDTH          960
#define BILL_CFG_FULLSCREEN     false

#if _CLI_DEV_MODE
# define BILL_CFG_DEV_MODE      true
#else
# define BILL_CFG_DEV_MODE      false
#endif

#include "sdl_renderer.cpp"
#include "bill.cpp"

int main(int, char**)
{
    //
    // SDL init
    //
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        printf("Failed to initialize SDL!\n");
        printf("SDL message: %s!\n", SDL_GetError());
        return -1;
    }
    
    Uint32 windowFlags = SDL_WINDOW_SHOWN;
    if (BILL_CFG_FULLSCREEN)
    {
        windowFlags |= SDL_WINDOW_FULLSCREEN;
    }

    SDL_Window *window = SDL_CreateWindow(BILL_CFG_WINDOW_TITLE, 
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            BILL_CFG_WIDTH, BILL_CFG_HEIGHT,
                            windowFlags);
    
    if (window == NULL)
    {
        printf("Failed to create window\n");
        printf("SDL message: %s\n", SDL_GetError());
        return -1;
    }
    
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (surface == NULL)
    {
        printf("Failed to get window surface\n");
        printf("SDL message: %s\n", SDL_GetError());
        return -1;
    }
    
    SDL_Renderer *sdlRenderer = SDL_CreateSoftwareRenderer(surface);
    if (sdlRenderer == NULL)
    {
        printf("Failed to create software sdl_renderer\n");
        printf("SDL message: %s\n", SDL_GetError());
        return -1;
    }
    
    S32 targetFramesPerSeconds = BILL_CFG_FPS;
    S32 targetMsPerFrame = 1000 / targetFramesPerSeconds;
    
    //
    // dt
    //
    Tick tick = {};
    tick.start = SDL_GetTicks();
    tick.end = tick.start;
    tick.dt = (S32)(tick.end - tick.start);
    
    //
    // memory
    //
    GameStorage storage = {};
    storage.permanSize = PLATFORM_PERMANENT_STRG_SZ;
    storage.persistSize = PLATFORM_PERSISTENT_STRG_SZ;
    size_t commonMemBlockSz = storage.permanSize + storage.persistSize;
    U8 *commonMemBlockPtr = (U8*)SDL_malloc(commonMemBlockSz);
    MemoryZero(commonMemBlockPtr, commonMemBlockSz);
    storage.permanent = commonMemBlockPtr;
    storage.persistent = commonMemBlockPtr + storage.permanSize;

    // 
    // renderer
    // 
    RendererHandle hRenderer = {};
    hRenderer.ctx = (void*)sdlRenderer;
    hRenderer.wScreen = BILL_CFG_WIDTH;
    hRenderer.hScreen = BILL_CFG_HEIGHT;

    //
    // input
    //
    InputDevices devices = {};

    //
    // gameIO
    //
    GameIO io = {};
    io.devices = &devices;
    io.hRenderer = &hRenderer;
    io.storage = &storage;
    io.tick = &tick;

    // 
    // debug
    //
    dbg_SdlRenderer = sdlRenderer;
    dbg_Window = window;
    dbg_HRenderer = &hRenderer;
    dbg_GameIO = &io;
    
    //
    // Event loop
    //
    SDL_Event event = {};
    B32 quitFlag = false;
    while (!quitFlag)
    {
        SDL_RenderClear(sdlRenderer);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                {
                    // NOTE(annad): Alt+F4 work too.
                    quitFlag = true;
                } break;
                
                case SDL_KEYUP:
                {
                    if (event.key.keysym.sym == SDLK_RETURN)
                    {
                        devices.keybBtns[KEYB_BTN_RETURN] = false;
                    }
                } break;
                
                case SDL_KEYDOWN:
                {
                    if (event.key.keysym.sym == SDLK_RETURN)
                    {
                        devices.keybBtns[KEYB_BTN_RETURN] = true;
                    }
                } break;

                case SDL_MOUSEBUTTONUP:
                {
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        devices.mouseBtns[MOUSE_BTN_LEFT] = false;
                    }
                    
                    if (event.button.button == SDL_BUTTON_MIDDLE)
                    {
                        devices.mouseBtns[MOUSE_BTN_MIDDLE] = false;
                    }
                    
                    if (event.button.button == SDL_BUTTON_RIGHT)
                    {
                        devices.mouseBtns[MOUSE_BTN_MIDDLE] = false;
                    }
                } break;

                case SDL_MOUSEBUTTONDOWN:
                { 
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        devices.mouseBtns[MOUSE_BTN_LEFT] = true;
                    }
                    
                    if (event.button.button == SDL_BUTTON_MIDDLE)
                    {
                        devices.mouseBtns[MOUSE_BTN_MIDDLE] = true;
                    }
                    
                    if (event.button.button == SDL_BUTTON_RIGHT)
                    {
                        devices.mouseBtns[MOUSE_BTN_MIDDLE] = true;
                    }
                } break;
                                
                case SDL_MOUSEMOTION:
                {
                    devices.mouseX = event.motion.x;
                    devices.mouseY = event.motion.y;
                } break;
            }
        }
        
        gtick(&io);
        SDLRenderer_exec(io.hRenderer);
        SDL_RenderPresent(sdlRenderer);
        SDL_UpdateWindowSurface(window);
        
        //
        // time
        //
        tick.end = SDL_GetTicks();
        tick.dt = (S32)(tick.end - tick.start);
        S32 frame_delay_time = (Uint32)(targetMsPerFrame - tick.dt);
        if(frame_delay_time > 0)
        {
            SDL_Delay((Uint32)frame_delay_time);
        }
        
        tick.end = SDL_GetTicks();
        tick.dt = (S32)(tick.end - tick.start);
        while(tick.dt < targetMsPerFrame)
        {
            tick.end = SDL_GetTicks();
            tick.dt = (S32)(tick.end - tick.start);
        }
        
        tick.start = tick.end;
    }
    
    free(commonMemBlockPtr);
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

