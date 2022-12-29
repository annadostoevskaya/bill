/* 
Author: github.com/annadostoevskaya
File: sdl_bill.cpp
Date: September 19th 2022 10:19 pm 

Description: <empty>
*/

#include "base_types.h"
#include "arena.cpp"

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

const U8 WINDOW_TITLE[] = "billards";
const U32 WINDOW_WIDTH = 960;
const U32 WINDOW_HEIGHT = 540;
const U32 BILL_FPS = 30;

int main(int, char**)
{
    //
    // sdl init
    //
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        printf("Failed to initialize SDL!\n");
        printf("SDL message: %s!\n", SDL_GetError());
        return -1;
    }
    
    SDL_Window *window = SDL_CreateWindow((const char *)WINDOW_TITLE, 
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          WINDOW_WIDTH, 
                                          WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    
    if(window == NULL)
    {
        printf("Failed to create window\n");
        printf("SDL message: %s\n", SDL_GetError());
        return -1;
    }
    
    DEBUG_window = window;
    
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if(surface == NULL)
    {
        printf("Failed to get window surface\n");
        printf("SDL message: %s\n", SDL_GetError());
        return -1;
    }
    
    SDL_Renderer *sdl_renderer = SDL_CreateSoftwareRenderer(surface);
    DEBUG_sdl_renderer = sdl_renderer;
    if(sdl_renderer == NULL)
    {
        printf("Failed to create software sdl_renderer\n");
        printf("SDL message: %s\n", SDL_GetError());
        return -1;
    }
    
    B32 run = true;
    U64 target_frames_per_seconds = BILL_FPS;
    S64 target_milliseconds_per_frame = 1000 / target_frames_per_seconds;
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
    
    renderer.context.width = WINDOW_WIDTH;
    renderer.context.height = WINDOW_HEIGHT;
    
    //
    // input
    //
    GameInput game_input = {};
    
    while(run)
    {
        SDL_RenderClear(sdl_renderer);
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
                    run = false;
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
        renderer_sdl_execute(sdl_renderer, &renderer);
        SDL_RenderPresent(sdl_renderer);
        SDL_UpdateWindowSurface(window);
        
        //
        // time
        //
        game_time.end = SDL_GetTicks();
        game_time.dt = game_time.end - game_time.start;
        S32 frame_delay_time = (Uint32)(target_milliseconds_per_frame - game_time.dt);
        if(frame_delay_time > 0)
        {
            SDL_Delay((Uint32)frame_delay_time);
        }
        
        game_time.end = SDL_GetTicks();
        game_time.dt = game_time.end - game_time.start;
        while(game_time.dt < target_milliseconds_per_frame)
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

