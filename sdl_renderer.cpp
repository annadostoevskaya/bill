/* 
Author: github.com/annadostoevskaya
File: sdl_renderer.cpp
Date: September 29th 2022 9:14 pm 

Description: <empty>
*/
// #pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wvarargs"
// #include "bill_renderer.h"

/*
void renderer_sdl_draw_fill_rect(SDL_Renderer *sdl_renderer, Renderer *renderer)
{
    RendererCommands *renderer_commands = &renderer->commands;
    
    // NOTE(annad): Out of executable memory!
    Assert(renderer_commands->peak_ptr >= renderer_commands->queue_ptr + (sizeof(S32) * 4));
    
    S32 *args_ptr = (S32*)(renderer_commands_get_current_queue_ptr(renderer_commands));
    S32 x = args_ptr[0];
    S32 y = args_ptr[1];
    S32 w = args_ptr[2];
    S32 h = args_ptr[3];
    
    renderer_commands->queue_ptr += (sizeof(S32) * 4);
    
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(sdl_renderer, &rect);
    
    LogRendererPop("RENDERER_COMMAND_DRAW_FILL_RECT", renderer_commands->queue_ptr);
}

void renderer_sdl_set_renderer_draw_color(SDL_Renderer *sdl_renderer, Renderer *renderer)
{
    RendererCommands *renderer_commands = &renderer->commands;
    // NOTE(annad): Out of executable memory side!
    Assert(renderer_commands->peak_ptr >= renderer_commands->queue_ptr + (sizeof(Uint8) * 4));
    
    Uint8 *args_ptr = (Uint8*)(renderer_commands_get_current_queue_ptr(renderer_commands));
    Uint8 r = args_ptr[0];
    Uint8 g = args_ptr[1];
    Uint8 b = args_ptr[2];
    Uint8 a = args_ptr[3];
    
    renderer_commands->queue_ptr += (sizeof(Uint8) * 4);
    SDL_SetRenderDrawColor(sdl_renderer, r, g, b, a);
    
    LogRendererPop("RENDERER_COMMAND_SET_RENDER_COLOR", renderer_commands->queue_ptr);
}

void renderer_sdl_draw_point(SDL_Renderer *sdl_renderer, Renderer *renderer)
{
    RendererCommands *renderer_commands = &renderer->commands;
    // NOTE(annad): Out of executable memory side!
    Assert(renderer_commands->peak_ptr >= renderer_commands->queue_ptr + (2 * sizeof(S32)));
    
    S32 *args_ptr = (S32*)(renderer_commands_get_current_queue_ptr(renderer_commands));
    S32 x = args_ptr[0];
    S32 y = args_ptr[1];
    
    renderer_commands->queue_ptr += (2 * sizeof(S32));
    SDL_RenderDrawPoint(sdl_renderer, x, y);
    
    LogRendererPop("RENDERER_COMMAND_DRAW_POINT", renderer_commands->queue_ptr);
}

void renderer_sdl_draw_line(SDL_Renderer *sdl_renderer, Renderer *renderer)
{
    RendererCommands *renderer_commands = &renderer->commands;
    // NOTE(annad): Out of executable memory side!
    Assert(renderer_commands->peak_ptr >= renderer_commands->queue_ptr + (4 * sizeof(S32)));
    
    S32 *args_ptr = (S32*)(renderer_commands_get_current_queue_ptr(renderer_commands));
    S32 x1 = args_ptr[0];
    S32 y1 = args_ptr[1];
    S32 x2 = args_ptr[2];
    S32 y2 = args_ptr[3];
    
    renderer_commands->queue_ptr += (4 * sizeof(S32));
    SDL_RenderDrawLine(sdl_renderer, x1, y1, x2, y2);
    
    LogRendererPop("RENDERER_COMMAND_DRAW_LINE", renderer_commands->queue_ptr);
}

void renderer_sdl_draw_circle(SDL_Renderer *sdl_renderer, Renderer *renderer)
{
    RendererCommands *renderer_commands = &renderer->commands;
    // NOTE(annad): Out of executable memory side!
    Assert(renderer_commands->peak_ptr >= renderer_commands->queue_ptr + (3 * sizeof(S32)));
    
    S32 *args_ptr = (S32*)(renderer_commands_get_current_queue_ptr(renderer_commands));
    S32 centreX = args_ptr[0];
    S32 centreY = args_ptr[1];
    S32 r = args_ptr[2];
    
    //
    // Midpoint Circle Algorithm.
    //
    
    S32 d = 2 * r;
    S32 x = (r - 1);
    S32 y = 0;
    S32 tx = 1; 
    S32 ty = 1;
    S32 err = (tx - d);
    
    while(x >= y)
    {
        SDL_RenderDrawPoint(sdl_renderer, centreX + x, centreY - y);
        SDL_RenderDrawPoint(sdl_renderer, centreX + x, centreY + y);
        SDL_RenderDrawPoint(sdl_renderer, centreX - x, centreY - y);
        SDL_RenderDrawPoint(sdl_renderer, centreX - x, centreY + y);
        SDL_RenderDrawPoint(sdl_renderer, centreX + y, centreY - x);
        SDL_RenderDrawPoint(sdl_renderer, centreX + y, centreY + x);
        SDL_RenderDrawPoint(sdl_renderer, centreX - y, centreY - x);
        SDL_RenderDrawPoint(sdl_renderer, centreX - y, centreY + x);
        
        if(err <= 0)
        {
            ++y; 
            err += ty;
            ty += 2;
        }
        
        if(err > 0)
        {
            --x;
            tx += 2;
            err += (tx - d);
        }
    }
    
    //
    // Midpoint Circle Algorithm.
    //
    
    renderer_commands->queue_ptr += (3 * sizeof(S32));
    
    LogRendererPop("RENDERER_COMMAND_DRAW_CIRCLE", renderer_commands->queue_ptr);
}
*/

void SDLRenderer_setDrawColor(RendererHandle *hRenderer, SDL_Renderer *sdlRend, U8 *cmd)
{
    // NOTE(annad): Error, out of memory!
    Assert(hRenderer->peak - sizeof(Renderer_Command) - 4*sizeof(U8) >= 0);
    // NOTE(annad): Error, invalid command code!
    Assert(*cmd == RCMD_SET_RENDER_COLOR);
    U8 r = *(cmd + sizeof(Renderer_Command) + 0);
    U8 g = *(cmd + sizeof(Renderer_Command) + 1);
    U8 b = *(cmd + sizeof(Renderer_Command) + 2);
    U8 a = *(cmd + sizeof(Renderer_Command) + 3);
    SDL_SetRenderDrawColor(sdlRend, r, g, b, a);
    hRenderer->peak -= (sizeof(Renderer_Command) + 4*sizeof(U8));
    cmd += (sizeof(Renderer_Command) + 4*sizeof(U8));
}

void SDLRenderer_exec(RendererHandle *hRenderer, SDL_Renderer *sdlRend)
{
    U8 *cmdPointer = hRenderer->byteCode;
    while (*cmdPointer)
    {
        switch (*cmdPointer)
        {
            case RCMD_SET_RENDER_COLOR:
            {
                SDLRenderer_setDrawColor(hRenderer, sdlRend, cmdPointer);
            } break;

            default: 
            {
                // NOTE(annad): Error, undefined behaviour.
                Assert(false);
            } break;
        }
        
        // !!!
        // NOTE(annad): Invalid cmdPointer calculating!!!
        // !!!
        Assert(false);
        U8 *cmdPointer = hRenderer->byteCode + (hRenderer->size - hRenderer->peak);
    }
}

/*
void renderer_sdl_execute(SDL_Renderer *sdl_renderer, 
                          Renderer *renderer)
{
    RendererCommands *renderer_commands = &(renderer->commands);
    renderer_commands->queue_ptr = 0;
    while(renderer_commands->queue_ptr < renderer_commands->peak_ptr)
    {
        // NOTE(annad): Out of executable side
        Assert(renderer_commands->peak_ptr > renderer_commands->queue_ptr + sizeof(Renderer_Command));
        
        S32 *p_command = (S32*)(renderer_commands_get_current_queue_ptr(renderer_commands));
        renderer_commands->queue_ptr += sizeof(Renderer_Command);
        switch(*p_command)
        {
            case RENDERER_COMMAND_DRAW_FILL_RECT:
            {
                renderer_sdl_draw_fill_rect(sdl_renderer, renderer);
                break;
            }
            
            case RENDERER_COMMAND_SET_RENDER_COLOR:
            {
                renderer_sdl_set_renderer_draw_color(sdl_renderer, renderer);
                break;
            }
            
            case RENDERER_COMMAND_DRAW_POINT:
            {
                renderer_sdl_draw_point(sdl_renderer, renderer);
                break;
            }
            
            case RENDERER_COMMAND_DRAW_LINE:
            {
                renderer_sdl_draw_line(sdl_renderer, renderer);
                break;
            }
            
            case RENDERER_COMMAND_DRAW_FILL_CIRCLE:
            {
                
            }
            
            case RENDERER_COMMAND_DRAW_CIRCLE:
            {
                renderer_sdl_draw_circle(sdl_renderer, renderer);
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
*/

// #pragma clang diagnostic pop

