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
# if !SDL_VERSION_ATLEAST(2,0,17)
# error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
# endif

# define IMGUI_IMPLEMENTATION 1
# include "dev/imgui/misc/single_file/imgui_single_file.h"
# include "dev/imgui/imgui_impl_sdlrenderer.h"
# include "dev/imgui/imgui_impl_sdlrenderer.cpp"
# include "dev/imgui/imgui_impl_sdl.h"
# include "dev/imgui/imgui_impl_sdl.cpp"
#endif // _DEVELOPER_MODE

//
//
//

void drawSquare(SDL_Renderer *renderer, S32 x, S32 y, S32 w, S32 h)
{
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}

//
//
//

enum Renderer_Command
{
    RENDERER_COMMAND_DRAW_FILL_RECT,
    RENDERER_COMMAND_SET_RENDER_COLOR,
    
    RENDERER_COMMAND_COUNT,
};

#define RENDERER_COMMAND_SIZE 256

typedef struct RendererCommands
{
    void *commands;
    size_t pointer;
    size_t size;
} RendererCommands;

void Renderer_drawFillRect(RendererCommands *renderer_commands, S32 x, S32 y, S32 w, S32 h)
{
    size_t draw_square_command_size = sizeof(Renderer_Command);
    size_t draw_square_arg_counter = 4;
    size_t draw_square_arg_size = sizeof(S32);
    
    // NOTE(annad): Out of memory!
    Assert(renderer_commands->pointer + 
           draw_square_command_size + 
           draw_square_arg_size * 
           draw_square_arg_counter < renderer_commands->size);
    
    S32 *args_ptr = (S32*)(((U8*)renderer_commands->commands) + renderer_commands->pointer);
    args_ptr[0] = x;
    args_ptr[1] = y;
    args_ptr[2] = w;
    args_ptr[3] = h;
    
    renderer_commands->pointer += (draw_square_arg_counter * draw_square_arg_size);
    
    // TODO(annad): use Renderer_Command* (pointer)
    S32 *p_commands = (S32*)(((U8*)renderer_commands->commands) + renderer_commands->pointer);
    *p_commands = RENDERER_COMMAND_DRAW_FILL_RECT;
    renderer_commands->pointer += draw_square_command_size;
    
#if defined(_DEVELOPER_MODE)
    printf("[PUSH] ");
    EvalPrint(renderer_commands->pointer);
#endif
}

void Renderer_setRendererDrawColor(RendererCommands *renderer_commands, U8 r, U8 g, U8 b, U8 a)
{
    size_t command_size = sizeof(Renderer_Command);
    size_t arg_counter = 4;
    size_t arg_size = sizeof(U8);
    
    // NOTE(annad): Out of memory!
    Assert(renderer_commands->pointer + 
           command_size + 
           arg_size * 
           arg_counter < renderer_commands->size);
    
    U8 *args_ptr = (U8*)(((U8*)renderer_commands->commands) + renderer_commands->pointer);
    args_ptr[0] = r;
    args_ptr[1] = g;
    args_ptr[2] = b;
    args_ptr[3] = a;
    
    renderer_commands->pointer += (arg_counter * arg_size);
    
    S32 *p_commands = (S32*)(((U8*)renderer_commands->commands) + renderer_commands->pointer);
    *p_commands = RENDERER_COMMAND_SET_RENDER_COLOR;
    renderer_commands->pointer += command_size;
    
#if defined(_DEVELOPER_MODE)
    printf("[PUSH] ");
    EvalPrint(renderer_commands->pointer);
#endif
}

void RendererCommands_SDL_drawFillRect(SDL_Renderer *renderer, RendererCommands *renderer_commands)
{
    size_t draw_square_arg_counter = 4;
    size_t draw_square_arg_size = sizeof(S32);
    
    Assert(renderer_commands->pointer >= (draw_square_arg_counter * draw_square_arg_size));
    renderer_commands->pointer -= (draw_square_arg_counter * draw_square_arg_size);
    
    S32 *args_ptr = (S32*)(((U8*)renderer_commands->commands) + renderer_commands->pointer);
    S32 x = args_ptr[0];
    S32 y = args_ptr[1];
    S32 w = args_ptr[2];
    S32 h = args_ptr[3];
    
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
    
#if defined(_DEVELOPER_MODE)
    printf("[POP] ");
    EvalPrint(renderer_commands->pointer);
#endif 
}

void RendererCommand_SDL_setRendererDrawColor(SDL_Renderer *renderer, RendererCommands *renderer_commands)
{
    size_t arg_counter = 4;
    size_t arg_size = sizeof(Uint8);
    Assert(renderer_commands->pointer >= (arg_counter * arg_size));
    renderer_commands->pointer -= (arg_counter * arg_size);
    
    Uint8 *args_ptr = (Uint8*)(((U8*)renderer_commands->commands) + renderer_commands->pointer);
    Uint8 r = args_ptr[0];
    Uint8 g = args_ptr[1];
    Uint8 b = args_ptr[2];
    Uint8 a = args_ptr[3];
    
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    
#if defined(_DEVELOPER_MODE)
    printf("[POP] ");
    EvalPrint(renderer_commands->pointer);
#endif 
}

void RendererCommands_SDL_pop(SDL_Renderer *renderer, RendererCommands *renderer_commands)
{
    while(renderer_commands->pointer > 0)
    {
        Assert(renderer_commands->pointer > sizeof(Renderer_Command));
        
        renderer_commands->pointer -= sizeof(Renderer_Command);
        S32 *p_command = (S32*)(((U8*)renderer_commands->commands) + renderer_commands->pointer);
        
        switch(*p_command)
        {
            case RENDERER_COMMAND_DRAW_FILL_RECT:
            {
                RendererCommands_SDL_drawFillRect(renderer, renderer_commands);
                break;
            }
            
            case RENDERER_COMMAND_SET_RENDER_COLOR:
            {
                RendererCommand_SDL_setRendererDrawColor(renderer, renderer_commands);
                break;
            }
            
            default:
            {
                // NOTE(annad): Unknow renderer command!
                Assert(false);
            }
        }
    }
    
    Assert(renderer_commands->size > renderer_commands->pointer);
}

/* 
typedef struct RGBA_t
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
        
        U8 E[4];
    };
} RGBA_t;
 */

void RendererCommands_push(RendererCommands *renderer_commands, 
                           Renderer_Command renderer_command, ...)
{
    va_list argptr;
    va_start(argptr, renderer_command);
    
    switch(renderer_command)
    {
        case RENDERER_COMMAND_DRAW_FILL_RECT:
        {
            S32 x = va_arg(argptr, S32);
            S32 y = va_arg(argptr, S32);
            S32 w = va_arg(argptr, S32);
            S32 h = va_arg(argptr, S32);
            
            Renderer_drawFillRect(renderer_commands, x, y, w, h);
            
            break;
        }
        
        case RENDERER_COMMAND_SET_RENDER_COLOR:
        {
            U8 r = va_arg(argptr, U8);
            U8 g = va_arg(argptr, U8);
            U8 b = va_arg(argptr, U8);
            U8 a = va_arg(argptr, U8);
            
            Renderer_setRendererDrawColor(renderer_commands, r, g, b, a);
            
            break;
        }
        
        default: 
        {
            // NOTE(annad): Unknown command code.
            Assert(false);
        }
    }
    
    va_end(argptr);
}

#include "bill.cpp"

const U8 WINDOW_TITLE[] = "billards";
const U32 WINDOW_WIDTH = 960;
const U32 WINDOW_HEIGHT = 540;

int main(int, char**)
{
    printf(arge[1]);
    
    __debugbreak();
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
    SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);
    if(renderer == NULL)
    {
        printf("Failed to create software renderer\n");
        printf("SDL message: %s\n", SDL_GetError());
        return -1;
    }
    
    
    B32 run = true;
    U64 target_frames_per_seconds = 30;
    S64 target_milliseconds_per_frame = 1000 / target_frames_per_seconds;
    SDL_Event event;
    
#if defined(_DEVELOPER_MODE)
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    io.Fonts->AddFontFromFileTTF("../dev/imgui/misc/fonts/Inconsolata-Regular.ttf", 16.0f);
    
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);
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
    game_memory.permanent_storage = malloc(game_memory.permanent_storage_size);
    MemoryZero(game_memory.permanent_storage, game_memory.permanent_storage_size);
    
    game_memory.persistent_storage_size = MB(256);
    game_memory.persistent_storage = malloc(game_memory.persistent_storage_size);
    MemoryZero(game_memory.persistent_storage, game_memory.persistent_storage_size);
    
    //
    // memory
    //
    
    // 
    // renderer
    // 
    
    RendererCommands renderer_commands;
    renderer_commands.size = RENDERER_COMMAND_SIZE;
    renderer_commands.commands = malloc(renderer_commands.size);
    MemoryZero(renderer_commands.commands, renderer_commands.size);
    renderer_commands.pointer = 0;
    
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
        SDL_RenderClear(renderer);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
        
        while(SDL_PollEvent(&event))
        {
#if defined(_DEVELOPER_MODE)
            ImGui_ImplSDL2_ProcessEvent(&event);
#endif // _DEVELOPER_MODE
            
            switch(event.type)
            {
                // NOTE(annad): Alt+F4 work too.
                case SDL_QUIT:
                {
                    run = false;
                    break;
                }
                
                case SDL_KEYDOWN:
                {
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_RETURN:
                        {
                            __debugbreak();
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
                            game_input.mouse.buttons_states[MOUSE_BUTTON_LEFT].enum_state = BUTTON_STATE_DOWN;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_LEFT].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_LEFT].click_pos.y = event.button.y;
                            break;
                        }
                        
                        case SDL_BUTTON_MIDDLE:
                        {
                            game_input.mouse.buttons_states[MOUSE_BUTTON_MID].enum_state = BUTTON_STATE_DOWN;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_MID].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_MID].click_pos.y = event.button.y;
                            break;
                        }
                        
                        case SDL_BUTTON_RIGHT:
                        {
                            game_input.mouse.buttons_states[MOUSE_BUTTON_RIGHT].enum_state = BUTTON_STATE_DOWN;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_RIGHT].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_RIGHT].click_pos.y = event.button.y;
                            break;
                        }
                        
                        case SDL_BUTTON_X1:
                        {
                            game_input.mouse.buttons_states[MOUSE_BUTTON_X1].enum_state = BUTTON_STATE_DOWN;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_X1].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_X1].click_pos.y = event.button.y;
                            break;
                        }
                        
                        case SDL_BUTTON_X2:
                        {
                            game_input.mouse.buttons_states[MOUSE_BUTTON_X2].enum_state = BUTTON_STATE_DOWN;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_X2].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_X2].click_pos.y = event.button.y;
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
                            game_input.mouse.buttons_states[MOUSE_BUTTON_LEFT].enum_state = BUTTON_STATE_UP;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_LEFT].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_LEFT].click_pos.y = event.button.y;
                            break;
                        }
                        
                        case SDL_BUTTON_MIDDLE:
                        {
                            game_input.mouse.buttons_states[MOUSE_BUTTON_MID].enum_state = BUTTON_STATE_UP;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_MID].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_MID].click_pos.y = event.button.y;
                            break;
                        }
                        
                        case SDL_BUTTON_RIGHT:
                        {
                            game_input.mouse.buttons_states[MOUSE_BUTTON_RIGHT].enum_state = BUTTON_STATE_UP;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_RIGHT].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_RIGHT].click_pos.y = event.button.y;
                            break;
                        }
                        
                        case SDL_BUTTON_X1:
                        {
                            game_input.mouse.buttons_states[MOUSE_BUTTON_X1].enum_state = BUTTON_STATE_UP;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_X1].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_X1].click_pos.y = event.button.y;
                            break;
                        }
                        
                        case SDL_BUTTON_X2:
                        {
                            game_input.mouse.buttons_states[MOUSE_BUTTON_X2].enum_state = BUTTON_STATE_UP;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_X2].click_pos.x = event.button.x;
                            game_input.mouse.buttons_states[MOUSE_BUTTON_X2].click_pos.y = event.button.y;
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
#endif
        GameUpdateAndRender(&game_memory, &renderer_commands, &game_input, &game_time);
        RendererCommands_SDL_pop(renderer, &renderer_commands);
        
#if defined(_DEVELOPER_MODE)
        // Rendering
        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
#endif // _DEVELOPER_MODE
        
        SDL_RenderPresent(renderer);
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
        
        // F64 FPS = (1000.0f) / ((F64)(game_time.dt));
        // printf("[FPS] ");
        // EvalPrintF(FPS);
        
        game_time.start = game_time.end;
        
        //
        // time
        //
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}

