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

#define BILL_CFG_FPS            60
#define BILL_CFG_WINDOW_TITLE   "bill"
#define BILL_CFG_WIDTH          (1920/2)
#define BILL_CFG_HEIGHT         (1080/2)
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
    storage.assetsSize = PLATFORM_ASSETS_STRG_SZ;
    size_t commonMemBlockSz = storage.permanSize + storage.persistSize + storage.assetsSize;
    U8 *commonMemBlockPtr = (U8*)SDL_malloc(commonMemBlockSz);
    MemoryZero(commonMemBlockPtr, commonMemBlockSz);
    storage.permanent = commonMemBlockPtr;
    storage.persistent = commonMemBlockPtr + storage.permanSize;
    storage.assets = commonMemBlockPtr + storage.permanSize + storage.persistSize;

    //
    // assets
    //
    SDL_RWops *assetsBundle = SDL_RWFromFile("./assets.bundle", "rb");
    if (!assetsBundle)
    {
        printf("Failed to load assets.bundle\n");
        printf("SDL message: %s\n", SDL_GetError());
        return -1;
    }

    U64 assetsBundleSize = assetsBundle->size(assetsBundle);
    Assert(assetsBundleSize < storage.assetsSize);
    if (assetsBundle->read(assetsBundle, storage.assets, assetsBundleSize, 1) != 1)
    {
        printf("Failed to load assets.bundle");
        printf("SDL message: %s\n", SDL_GetError());
        return -1;
    }

    assetsBundle->close(assetsBundle);

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
    // screen
    //
    Screen screen = {};
    screen.w = surface->w;
    screen.h = surface->h;
    screen.pitch = surface->pitch;
    screen.buf = (U32*)surface->pixels;

    //
    // gameIO
    //
    GameIO io = {};
    io.devices = &devices;
    io.screen = &screen;
    io.hRenderer = &hRenderer;
    io.storage = &storage;
    io.tick = &tick;

#if BILL_CFG_DEV_MODE
    // 
    // debug
    //
    dbg_SdlRenderer = sdlRenderer;
    dbg_Window = window;
    dbg_HRenderer = &hRenderer;
    dbg_GameIO = &io;
    dbg_GlobalFrameCounter = 0;
#endif

    //
    // Event loop
    //
    SDL_Event event = {};
    B32 quitFlag = false;
    while (!quitFlag)
    {
#if BILL_CFG_DEV_MODE
        dbg_GlobalFrameCounter++;
#endif
        SDL_RenderClear(sdlRenderer);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));

        devices.dwheel = 0;
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

                    if (event.key.keysym.sym == SDLK_LSHIFT)
                    {
                        devices.keybBtns[KEYB_BTN_LSHIFT] = false;
                    }
                } break;
                
                case SDL_KEYDOWN:
                {
                    if (event.key.keysym.sym == SDLK_RETURN)
                    {
                        devices.keybBtns[KEYB_BTN_RETURN] = true;
                    }

                    if (event.key.keysym.sym == SDLK_LSHIFT)
                    {
                        devices.keybBtns[KEYB_BTN_LSHIFT] = true;
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

                case SDL_MOUSEWHEEL:
                {
                    devices.dwheel = event.wheel.y;
                } break;
            }
        }
        
        SDL_LockSurface(surface);
        gtick(&io);
        SDL_UnlockSurface(surface);

        // SDLRenderer_exec(io.hRenderer);
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
        
        // NOTE(annad): Profile
        F32 dt = (F32)tick.dt;
        F32 FPS = 1000.0f / dt;
        // printf("dt: %f, fps: %f\n", dt, FPS);

        tick.start = tick.end;
    }
    
    SDL_free(commonMemBlockPtr);
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

