/* 
Author: github.com/annadostoevskaya
File: sdl_bill.cpp
Date: September 19th 2022 10:19 pm 

Description: <empty>
*/

#include "base_types.h"
// #include "memory_arena.h"
#include "memory_arena.cpp"

#include <SDL2/SDL.h>
#include <Windows.h>
#include <stdarg.h>

#include "bill_platform.h"

#if defined(_DEVELOPER_MODE)
# if defined(_DEV_IMGUI_MODE)
#  if !SDL_VERSION_ATLEAST(2,0,17)
#   error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#  endif

#  define IMGUI_IMPLEMENTATION 1
#  include "dev/imgui/misc/single_file/imgui_single_file.h"
#  include "dev/imgui/imgui_impl_sdlrenderer.h"
#  include "dev/imgui/imgui_impl_sdlrenderer.cpp"
#  include "dev/imgui/imgui_impl_sdl.h"
#  include "dev/imgui/imgui_impl_sdl.cpp"
# endif // _DEV_IMGUI_MODE
#endif // _DEVELOPER_MODE

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
    union
    {
        struct 
        {
            S32 x;
            S32 y;
            S32 w;
            S32 h;
        };
        
        S32 e[4];
    };
} Rect;

#include "sdl_renderer.cpp"
#include "bill.cpp"

const U8 WINDOW_TITLE[] = "billards";
const U32 WINDOW_WIDTH = 960;
const U32 WINDOW_HEIGHT = 540;

int main(int, char**)
{
    /*
NOTE(annad):
1. SDL_RenderDrawRect
2. ImGui.
3. Base of Collision Detection for billiards.
*/
    
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
    
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if(surface == NULL)
    {
        printf("Failed to get window surface\n");
        printf("SDL message: %s\n", SDL_GetError());
        return -1;
    }
    
    // TODO(annad): It's for ImGui
    SDL_Renderer *sdl_renderer = SDL_CreateSoftwareRenderer(surface);
    if(sdl_renderer == NULL)
    {
        printf("Failed to create software sdl_renderer\n");
        printf("SDL message: %s\n", SDL_GetError());
        return -1;
    }
    
    
    B32 run = true;
    U64 target_frames_per_seconds = 30;
    S64 target_milliseconds_per_frame = 1000 / target_frames_per_seconds;
    SDL_Event event;
    
#if defined(_DEVELOPER_MODE)
# if defined(_DEV_IMGUI_MODE)
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    io.Fonts->AddFontFromFileTTF("../dev/imgui/misc/fonts/Inconsolata-Regular.ttf", 16.0f);
    
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, sdl_renderer);
    ImGui_ImplSDLRenderer_Init(sdl_renderer);
# endif // _DEV_IMGUI_MODE
#endif // _DEVELOPER_MODE
    
    //
    // dt
    //
    
    GameTime game_time;
    game_time.start = SDL_GetTicks();
    game_time.end = game_time.start;
    game_time.dt = game_time.end - game_time.start;
    
    //
    // dt
    //
    
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
    // memory
    //
    
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
    // renderer
    //
    
    //
    // input
    //
    GameInput game_input = {};
    
    //
    // input
    //
    
    while(run)
    {
        SDL_RenderClear(sdl_renderer);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
        
        while(SDL_PollEvent(&event))
        {
#if defined(_DEVELOPER_MODE)
# if defined(_DEV_IMGUI_MODE)
            ImGui_ImplSDL2_ProcessEvent(&event);
# endif // _DEV_IMGUI_MODE
#endif // _DEVELOPER_MODE
            
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
                        
                        case SDLK_w:
                        {
                            game_input.keyboard.keys[INPUT_KEYBOARD_KEYS_W] = INPUT_BUTTON_STATE_UP;
                            break;
                        }
                        
                        case SDLK_s:
                        {
                            game_input.keyboard.keys[INPUT_KEYBOARD_KEYS_S] = INPUT_BUTTON_STATE_UP;
                            break;
                        }
                        
                        case SDLK_a:
                        {
                            game_input.keyboard.keys[INPUT_KEYBOARD_KEYS_A] = INPUT_BUTTON_STATE_UP;
                            break;
                        }
                        
                        case SDLK_d:
                        {
                            game_input.keyboard.keys[INPUT_KEYBOARD_KEYS_D] = INPUT_BUTTON_STATE_UP;
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
                        
                        case SDLK_w:
                        {
                            game_input.keyboard.keys[INPUT_KEYBOARD_KEYS_W] = INPUT_BUTTON_STATE_DOWN;
                            break;
                        }
                        
                        case SDLK_s:
                        {
                            game_input.keyboard.keys[INPUT_KEYBOARD_KEYS_S] = INPUT_BUTTON_STATE_DOWN;
                            break;
                        }
                        
                        case SDLK_a:
                        {
                            game_input.keyboard.keys[INPUT_KEYBOARD_KEYS_A] = INPUT_BUTTON_STATE_DOWN;
                            break;
                        }
                        
                        case SDLK_d:
                        {
                            game_input.keyboard.keys[INPUT_KEYBOARD_KEYS_D] = INPUT_BUTTON_STATE_DOWN;
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
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_LEFT].enum_state = INPUT_BUTTON_STATE_DOWN;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_LEFT].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_LEFT].click_pos.y = event.button.y;
                            break;
                        }
                        
                        case SDL_BUTTON_MIDDLE:
                        {
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_MID].enum_state = INPUT_BUTTON_STATE_DOWN;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_MID].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_MID].click_pos.y = event.button.y;
                            break;
                        }
                        
                        case SDL_BUTTON_RIGHT:
                        {
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_RIGHT].enum_state = INPUT_BUTTON_STATE_DOWN;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_RIGHT].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_RIGHT].click_pos.y = event.button.y;
                            break;
                        }
                        
                        case SDL_BUTTON_X1:
                        {
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_X1].enum_state = INPUT_BUTTON_STATE_DOWN;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_X1].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_X1].click_pos.y = event.button.y;
                            break;
                        }
                        
                        case SDL_BUTTON_X2:
                        {
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_X2].enum_state = INPUT_BUTTON_STATE_DOWN;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_X2].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_X2].click_pos.y = event.button.y;
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
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_LEFT].enum_state = INPUT_BUTTON_STATE_UP;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_LEFT].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_LEFT].click_pos.y = event.button.y;
                            break;
                        }
                        
                        case SDL_BUTTON_MIDDLE:
                        {
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_MID].enum_state = INPUT_BUTTON_STATE_UP;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_MID].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_MID].click_pos.y = event.button.y;
                            break;
                        }
                        
                        case SDL_BUTTON_RIGHT:
                        {
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_RIGHT].enum_state = INPUT_BUTTON_STATE_UP;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_RIGHT].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_RIGHT].click_pos.y = event.button.y;
                            break;
                        }
                        
                        case SDL_BUTTON_X1:
                        {
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_X1].enum_state = INPUT_BUTTON_STATE_UP;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_X1].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_X1].click_pos.y = event.button.y;
                            break;
                        }
                        
                        case SDL_BUTTON_X2:
                        {
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_X2].enum_state = INPUT_BUTTON_STATE_UP;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_X2].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[INPUT_MOUSE_BUTTON_X2].click_pos.y = event.button.y;
                            break;
                        }
                    }
                    
                    break;
                }
                
                case SDL_MOUSEMOTION:
                {
                    game_input.mouse.cursor_pos.x = event.motion.x;
                    game_input.mouse.cursor_pos.y = event.motion.y;
                    break;
                }
            }
        }
        
#if defined(_DEVELOPER_MODE)
# if defined(_DEV_IMGUI_MODE)
        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        /*         
                // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
                bool Dev_ImGui_showDemoWindow = true;
                if (Dev_ImGui_showDemoWindow)
                    ImGui::ShowDemoWindow(&Dev_ImGui_showDemoWindow);
                 */
# endif // _DEV_IMGUI_MODE
#endif 
        GameUpdateAndRender(&game_memory, &renderer, &game_input, &game_time);
        Renderer_SDL_execute(sdl_renderer, &renderer);
        
#if defined(_DEVELOPER_MODE)
# if defined(_DEV_IMGUI_MODE)
        // Rendering
        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
# endif // _DEV_IMGUI_MODE
#endif // _DEVELOPER_MODE
        
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
        
        // #define _BILL_FPS_DEBUG
#if defined(_BILL_FPS_DEBUG)
        F64 FPS = (1000.0f) / ((F64)(game_time.dt));
        printf("[FPS] ");
        EvalPrintF(FPS);
#endif
        game_time.start = game_time.end;
        
        //
        // time
        //
    }
    
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}

