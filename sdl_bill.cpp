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


//
//
//


enum Renderer_Command
{
    RENDERER_COMMAND_NULL = 0,
    RENDERER_COMMAND_DRAW_FILL_RECT,
    RENDERER_COMMAND_SET_RENDER_COLOR,
    
    RENDERER_COMMAND_COUNT,
};

#define RENDERER_COMMAND_SIZE 0xff

typedef struct RendererCommands
{
    void *commands;
    size_t peak_ptr;
    size_t queue_ptr;
    size_t size;
} RendererCommands;

typedef struct RendererContext
{
    S32 width;
    S32 height;
} RendererContext;

typedef struct Renderer
{
    RendererCommands commands;
    RendererContext context;
} Renderer;

inline U8 *RendererCommands_getCurrentPeakPtr(RendererCommands *renderer_commands)
{
    return (((U8*)renderer_commands->commands) + renderer_commands->peak_ptr);
}

inline U8 *RendererCommands_getCurrentQueuePtr(RendererCommands *renderer_commands)
{
    return (((U8*)renderer_commands->commands) + renderer_commands->queue_ptr);
}

inline void RendererCommands_insertCommandInQueue(RendererCommands *renderer_commands, Renderer_Command command)
{
    S32 *p_commands = (S32*)(RendererCommands_getCurrentPeakPtr(renderer_commands));
    *p_commands = command;
}

void Renderer_drawFillRect(Renderer *renderer, Rect *rect)
{
    RendererCommands *renderer_commands = &renderer->commands;
    // NOTE(annad): Out of memory!
    Assert(renderer_commands->size > 
           renderer_commands->peak_ptr + 
           sizeof(Renderer_Command) +
           sizeof(Rect));
    
    RendererCommands_insertCommandInQueue(renderer_commands, RENDERER_COMMAND_DRAW_FILL_RECT);
    renderer_commands->peak_ptr += sizeof(Renderer_Command);
    
    S32 *args_ptr = (S32*)(RendererCommands_getCurrentPeakPtr(renderer_commands));
    args_ptr[0] = rect->x;
    args_ptr[1] = rect->y;
    args_ptr[2] = rect->w;
    args_ptr[3] = rect->h;
    renderer_commands->peak_ptr += sizeof(Rect);
    
#if defined(_DEVELOPER_MODE)
    printf("[PUSH] ");
    EvalPrint(renderer_commands->peak_ptr);
#endif
}

void Renderer_setRendererDrawColor(Renderer *renderer, RGBA_U8 *color)
{
    RendererCommands *renderer_commands = &renderer->commands;
    // NOTE(annad): Out of memory!
    Assert(renderer_commands->size > renderer_commands->peak_ptr + 
           sizeof(RGBA_U8) +
           sizeof(Renderer_Command));
    
    RendererCommands_insertCommandInQueue(renderer_commands, RENDERER_COMMAND_SET_RENDER_COLOR);
    renderer_commands->peak_ptr += sizeof(Renderer_Command);
    
    U8 *args_ptr = (U8*)(RendererCommands_getCurrentPeakPtr(renderer_commands));
    args_ptr[0] = color->r;
    args_ptr[1] = color->g;
    args_ptr[2] = color->b;
    args_ptr[3] = color->a;
    renderer_commands->peak_ptr += sizeof(RGBA_U8);
    
#if defined(_DEVELOPER_MODE)
    printf("[PUSH] ");
    EvalPrint(renderer_commands->peak_ptr);
#endif
}

void Renderer_SDL_drawFillRect(SDL_Renderer *sdl_renderer, Renderer *renderer)
{
    RendererCommands *renderer_commands = &renderer->commands;
    
    // NOTE(annad): Out of executable memory!
    Assert(renderer_commands->peak_ptr >= renderer_commands->queue_ptr + (sizeof(S32) * 4));
    
    S32 *args_ptr = (S32*)(RendererCommands_getCurrentQueuePtr(renderer_commands));
    S32 x = args_ptr[0];
    S32 y = args_ptr[1];
    S32 w = args_ptr[2];
    S32 h = args_ptr[3];
    
    renderer_commands->queue_ptr += (sizeof(S32) * 4);
    
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(sdl_renderer, &rect);
    
#if defined(_DEVELOPER_MODE)
    printf("[POP] ");
    EvalPrint(renderer_commands->queue_ptr);
#endif 
}

void Renderer_SDL_setRendererDrawColor(SDL_Renderer *sdl_renderer, 
                                       Renderer *renderer)
{
    RendererCommands *renderer_commands = &renderer->commands;
    // NOTE(annad): Out of executable memory side!
    Assert(renderer_commands->peak_ptr >= renderer_commands->queue_ptr + (sizeof(Uint8) * 4));
    
    Uint8 *args_ptr = (Uint8*)(RendererCommands_getCurrentQueuePtr(renderer_commands));
    Uint8 r = args_ptr[0];
    Uint8 g = args_ptr[1];
    Uint8 b = args_ptr[2];
    Uint8 a = args_ptr[3];
    
    renderer_commands->queue_ptr += (sizeof(Uint8) * 4);
    SDL_SetRenderDrawColor(sdl_renderer, r, g, b, a);
    
#if defined(_DEVELOPER_MODE)
    printf("[POP] ");
    EvalPrint(renderer_commands->queue_ptr);
#endif 
}

void Renderer_SDL_pop(SDL_Renderer *sdl_renderer, 
                      Renderer *renderer)
{
    RendererCommands *renderer_commands = &renderer->commands;
    renderer_commands->queue_ptr = 0;
    while(renderer_commands->queue_ptr < renderer_commands->peak_ptr)
    {
        // NOTE(annad): Out of executable side
        Assert(renderer_commands->peak_ptr > renderer_commands->queue_ptr + sizeof(Renderer_Command));
        
        S32 *p_command = (S32*)(RendererCommands_getCurrentQueuePtr(renderer_commands));
        renderer_commands->queue_ptr += sizeof(Renderer_Command);
        switch(*p_command)
        {
            case RENDERER_COMMAND_DRAW_FILL_RECT:
            {
                Renderer_SDL_drawFillRect(sdl_renderer, renderer);
                break;
            }
            
            case RENDERER_COMMAND_SET_RENDER_COLOR:
            {
                Renderer_SDL_setRendererDrawColor(sdl_renderer, renderer);
                break;
            }
            
            case RENDERER_COMMAND_NULL:
            {
                // NOTE(annad): NULL render command.
                Assert(false);
                break;
            }
            
            default:
            {
                // NOTE(annad): Unknow renderer command!
                Assert(false);
            }
        }
    }
    
    renderer_commands->peak_ptr = 0;
    Assert(renderer_commands->size > renderer_commands->peak_ptr);
}

void Renderer_pushCommand(Renderer *renderer, 
                          Renderer_Command command, ...)
{
    va_list argptr;
    va_start(argptr, command);
    
    switch(command)
    {
        case RENDERER_COMMAND_DRAW_FILL_RECT:
        {
            Rect *rect = va_arg(argptr, Rect*);
            Renderer_drawFillRect(renderer, rect);
            break;
        }
        
        case RENDERER_COMMAND_SET_RENDER_COLOR:
        {
            RGBA_U8 *color = va_arg(argptr, RGBA_U8*);
            Renderer_setRendererDrawColor(renderer, color);
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
    
    Renderer renderer = {};
    renderer.commands.size = RENDERER_COMMAND_SIZE;
    renderer.commands.commands = malloc(renderer.commands.size);
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
        GameUpdateAndRender(&game_memory, &renderer, &game_input, &game_time);
        Renderer_SDL_pop(sdl_renderer, &renderer);
        
#if defined(_DEVELOPER_MODE)
        // Rendering
        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
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
        
        // F64 FPS = (1000.0f) / ((F64)(game_time.dt));
        // printf("[FPS] ");
        // EvalPrintF(FPS);
        
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

