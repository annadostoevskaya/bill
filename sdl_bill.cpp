/* 
Author: github.com/annadostoevskaya
File: sdl_bill.cpp
Date: September 19th 2022 10:19 pm 

Description: <empty>
*/

#include "core/types.h"
#include "core/memory.h"
#include "core/memory.cpp"
#include "core/memory_void.cpp"

#include <SDL2/SDL.h>
#if _OS_WINDOWS
# include <Windows.h>
#endif
#include <stdarg.h>

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

typedef struct Rect
{
    S32 x;
    S32 y;
    S32 w;
    S32 h;
} Rect;

#include "sdl_renderer.cpp"
#include "bill.cpp"

#define BILL_CFG_FPS            30
#define BILL_CFG_WINDOW_TITLE   "bill"
#define BILL_CFG_HEIGHT         540
#define BILL_CFG_WIDTH          960
#define BILL_CFG_FULLSCREEN     false

#if _CLI_ENABLED_ASSERTS
# define BILL_CFG_ASSERTS       true
#else
# define BILL_CFG_ASSERTS       false
#endif 

#if _CLI_DEV_MODE
# define BILL_CFG_DEV_MODE      true
#else
# define BILL_CFG_DEV_MODE      false
#endif

int main(int, char**)
{
    EvalPrint(BILL_CFG_DEV_MODE);
    EvalPrint(BILL_CFG_ASSERTS);
    
    //
    // sdl init
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
                            BILL_CFG_WIDTH, 
                            BILL_CFG_HEIGHT,
                            windowFlags);
    dbg_Window = window;
    
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
    dbg_SdlRenderer = sdlRenderer; 
    if (sdlRenderer == NULL)
    {
        printf("Failed to create software sdl_renderer\n");
        printf("SDL message: %s\n", SDL_GetError());
        return -1;
    }
    
    B32 quitFlag = false;
    S32 targetFramesPerSeconds = BILL_CFG_FPS;
    S32 targetMsPerFrame = 1000 / targetFramesPerSeconds;
    SDL_Event event;
    
    //
    // dt
    //
    GameTime game_time;
    game_time.start = SDL_GetTicks();
    game_time.end = game_time.start;
    game_time.dt = game_time.end - game_time.start;
    
    //
    // memory
    //
    GameMemory game_memory;
    game_memory.permanent_storage_size = KB(4);
    game_memory.persistent_storage_size = MB(256);
    
    void *common_mem_pull = malloc(game_memory.permanent_storage_size 
                                   + game_memory.persistent_storage_size
                                   + RENDERER_COMMAND_BUFFER_SIZE);
    
    game_memory.permanent_storage = common_mem_pull;
    MemoryZero(game_memory.permanent_storage, game_memory.permanent_storage_size);
    
    game_memory.persistent_storage = ((U8*)common_mem_pull) + game_memory.permanent_storage_size;
    MemoryZero(game_memory.persistent_storage, game_memory.persistent_storage_size);
    
    // 
    // renderer
    // 
    Renderer renderer = {};
    renderer.commands.size = RENDERER_COMMAND_BUFFER_SIZE;
    renderer.commands.commands = ((U8*)common_mem_pull) 
        + game_memory.permanent_storage_size 
        + game_memory.persistent_storage_size;
    MemoryZero(renderer.commands.commands, renderer.commands.size);
    renderer.commands.peak_ptr = 0;
    renderer.commands.queue_ptr = 0;
    
    renderer.context.width = BILL_CFG_WIDTH;
    renderer.context.height = BILL_CFG_HEIGHT;
    
    //
    // input
    //
    GameInput game_input = {};
    
    while(!quitFlag)
    {
        SDL_RenderClear(sdlRenderer);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
        while(SDL_PollEvent(&event))
        {
            GameInputMouseButtonState *mouse_buttons = game_input.mouse.buttons_states;
            Vec2Dim<S32> *mouse_pos = &game_input.mouse.cursor_pos;
            
            switch(event.type)
            {
                // NOTE(annad): Alt+F4 work too.
                case SDL_QUIT:
                {
                    quitFlag = false;
                    break;
                }
                
                case SDL_KEYUP:
                {
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_RETURN:
                        {
                            game_input.keyboard.keys[INPUT_KEYBOARD_KEYS_RETURN] = INPUT_BUTTON_STATE_UP;
                            break;
                        }
                    }
                    
                    break;
                }
                
                case SDL_KEYDOWN:
                {
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_RETURN:
                        {
                            game_input.keyboard.keys[INPUT_KEYBOARD_KEYS_RETURN] = INPUT_BUTTON_STATE_DOWN;
                            break;
                        }
                    }
                    
                    break;
                }
                
                case SDL_MOUSEBUTTONDOWN:
                { 
                    switch(event.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                        {
                            set_mouse_button_state(&mouse_buttons[INPUT_MOUSE_BUTTON_LEFT],
                                                   INPUT_BUTTON_STATE_DOWN, event.button.x, event.button.y);
                            break;
                        }
                        
                        case SDL_BUTTON_MIDDLE:
                        {
                            set_mouse_button_state(&mouse_buttons[INPUT_MOUSE_BUTTON_MID],
                                                   INPUT_BUTTON_STATE_DOWN, event.button.x, event.button.y);
                            break;
                        }
                        
                        case SDL_BUTTON_RIGHT:
                        {
                            set_mouse_button_state(&mouse_buttons[INPUT_MOUSE_BUTTON_RIGHT],
                                                   INPUT_BUTTON_STATE_DOWN, event.button.x, event.button.y);
                            break;
                        }
                    }
                    
                    break;
                }
                
                case SDL_MOUSEBUTTONUP:
                {
                    switch(event.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                        {
                            set_mouse_button_state(&mouse_buttons[INPUT_MOUSE_BUTTON_LEFT],
                                                   INPUT_BUTTON_STATE_UP, event.button.x, event.button.y);
                            break;
                        }
                        
                        case SDL_BUTTON_MIDDLE:
                        {
                            set_mouse_button_state(&mouse_buttons[INPUT_MOUSE_BUTTON_MID],
                                                   INPUT_BUTTON_STATE_UP, event.button.x, event.button.y);
                            break;
                        }
                        
                        case SDL_BUTTON_RIGHT:
                        {
                            set_mouse_button_state(&mouse_buttons[INPUT_MOUSE_BUTTON_RIGHT],
                                                   INPUT_BUTTON_STATE_UP, event.button.x, event.button.y);
                            break;
                        }
                    }
                    
                    break;
                }
                
                case SDL_MOUSEMOTION:
                {
                    mouse_pos->x = event.motion.x;
                    mouse_pos->y = event.motion.y;
                    break;
                }
            }
        }
        
        game_update_and_render(&game_memory, &renderer, &game_input, &game_time);
        renderer_sdl_execute(sdlRenderer, &renderer);
        SDL_RenderPresent(sdl_renderer);
        SDL_UpdateWindowSurface(window);
        
        //
        // time
        //
        game_time.end = SDL_GetTicks();
        game_time.dt = game_time.end - game_time.start;
        S32 frame_delay_time = (Uint32)(targetMsPerFrame - game_time.dt);
        if(frame_delay_time > 0)
        {
            SDL_Delay((Uint32)frame_delay_time);
        }
        
        game_time.end = SDL_GetTicks();
        game_time.dt = game_time.end - game_time.start;
        while(game_time.dt < targetMsPerFrame)
        {
            game_time.end = SDL_GetTicks();
            game_time.dt = game_time.end - game_time.start;
        }
        
        game_time.start = game_time.end;
    }
    
    free(common_mem_pull);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

