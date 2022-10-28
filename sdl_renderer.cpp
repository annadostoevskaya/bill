/* 
Author: github.com/annadostoevskaya
File: sdl_renderer.cpp
Date: September 29th 2022 9:14 pm 

Description: <empty>
*/

#include "bill_renderer.h"

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
    
    LogRendererPop("RENDERER_COMMAND_DRAW_FILL_RECT", renderer_commands->queue_ptr);
}

void Renderer_SDL_setRendererDrawColor(SDL_Renderer *sdl_renderer, Renderer *renderer)
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
    
    LogRendererPop("RENDERER_COMMAND_SET_RENDER_COLOR", renderer_commands->queue_ptr);
}

void Renderer_SDL_drawPoint(SDL_Renderer *sdl_renderer, Renderer *renderer)
{
    RendererCommands *renderer_commands = &renderer->commands;
    // NOTE(annad): Out of executable memory side!
    Assert(renderer_commands->peak_ptr >= renderer_commands->queue_ptr + (2 * sizeof(S32)));
    
    S32 *args_ptr = (S32*)(RendererCommands_getCurrentQueuePtr(renderer_commands));
    S32 x = args_ptr[0];
    S32 y = args_ptr[1];
    
    renderer_commands->queue_ptr += (2 * sizeof(S32));
    SDL_RenderDrawPoint(sdl_renderer, x, y);
    
    LogRendererPop("RENDERER_COMMAND_DRAW_POINT", renderer_commands->queue_ptr);
}

void Renderer_SDL_drawLine(SDL_Renderer *sdl_renderer, Renderer *renderer)
{
    RendererCommands *renderer_commands = &renderer->commands;
    // NOTE(annad): Out of executable memory side!
    Assert(renderer_commands->peak_ptr >= renderer_commands->queue_ptr + (4 * sizeof(S32)));
    
    S32 *args_ptr = (S32*)(RendererCommands_getCurrentQueuePtr(renderer_commands));
    S32 x1 = args_ptr[0];
    S32 y1 = args_ptr[1];
    S32 x2 = args_ptr[2];
    S32 y2 = args_ptr[3];
    
    renderer_commands->queue_ptr += (4 * sizeof(S32));
    SDL_RenderDrawLine(sdl_renderer, x1, y1, x2, y2);
    
    LogRendererPop("RENDERER_COMMAND_DRAW_LINE", renderer_commands->queue_ptr);
}

void Renderer_SDL_drawCircle(SDL_Renderer *sdl_renderer, Renderer *renderer)
{
    RendererCommands *renderer_commands = &renderer->commands;
    // NOTE(annad): Out of executable memory side!
    Assert(renderer_commands->peak_ptr >= renderer_commands->queue_ptr + (3 * sizeof(S32)));
    
    S32 *args_ptr = (S32*)(RendererCommands_getCurrentQueuePtr(renderer_commands));
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

void Renderer_SDL_execute(SDL_Renderer *sdl_renderer, 
                          Renderer *renderer)
{
    RendererCommands *renderer_commands = &(renderer->commands);
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
            
            case RENDERER_COMMAND_DRAW_POINT:
            {
                Renderer_SDL_drawPoint(sdl_renderer, renderer);
                break;
            }
            
            case RENDERER_COMMAND_DRAW_LINE:
            {
                Renderer_SDL_drawLine(sdl_renderer, renderer);
                break;
            }
            
            case RENDERER_COMMAND_DRAW_FILL_CIRCLE:
            {
                
            }
            
            case RENDERER_COMMAND_DRAW_CIRCLE:
            {
                Renderer_SDL_drawCircle(sdl_renderer, renderer);
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

