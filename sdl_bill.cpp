/* 
Author: github.com/annadostoevskaya
File: sdl_bill.cpp
Date: September 19th 2022 10:19 pm 

Description: <empty>
*/

#include "base_types.h"
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <Windows.h>

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

typedef struct MemArena
{
    void *base;
    U64 size;
    U64 pos;
} MemArena;

MemArena MemArena_stdlib_make_mem_arena(U64 mem_size)
{
    MemArena result;
    
    result.base = malloc(mem_size);
    result.size = mem_size;
    result.pos = 0;
    
    MemoryZero(result.base, result.size);
    
    return result;
}

void MemArena_stdlib_free_mem_arena(MemArena *mem_arena)
{
    free(mem_arena->base);
}

void *MemArena_push(MemArena* mem_arena, U64 mem_size)
{
    void *result = 0;
    if(mem_arena->pos + mem_size < mem_arena->size)
    {
        result = (void*)(((U8*)mem_arena->base) + mem_arena->pos);
        mem_arena->pos += mem_size;
    }
    
    return result;
}

void MemArena_pop(MemArena *mem_arena, U64 pos)
{
    if(mem_arena->pos > pos && pos >= 0)
    {
        mem_arena->pos = pos;
    }
}

#define MemArena_PushStruct(mem_arena, T)       MemArena_push(mem_arena, sizeof(T))
#define MemArena_PushArray(mem_arena, T, count) MemArena_push(mem_arena, sizeof(T) * count)

void DrawSquare(SDL_Renderer *renderer, S32 x, S32 y, S32 w, S32 h)
{
    SDL_Rect rect = {x, y, w, h};
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void GameUpdateAndRender()
{
    
}

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
    SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);
    if(renderer == NULL)
    {
        printf("Failed to create software renderer\n");
        printf("SDL message: %s\n", SDL_GetError());
        return -1;
    }
    
    
    B32 Run = true;
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
    
    while(Run)
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
                    Run = false;
                    break;
                }
            }
        }
        
#if defined(_DEVELOPER_MODE)
        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        bool show_demo_window = true;
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
#endif
        
        GameUpdateAndRender();
        DrawSquare(renderer, 0, 0, 100, 100);
        
#if defined(_DEVELOPER_MODE)
        // Rendering
        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
#endif // _DEVELOPER_MODE
        
        SDL_UpdateWindowSurface(window);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
