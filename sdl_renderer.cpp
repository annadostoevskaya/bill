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
    
#if defined(_BILL_RENDERER_DEBUG_MODE)
# if defined(_DEVELOPER_MODE)
    printf("[POP] ");
    EvalPrint(renderer_commands->queue_ptr);
# endif 
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
#if defined(_BILL_RENDERER_DEBUG_MODE)
# if defined(_DEVELOPER_MODE)
    printf("[POP] ");
    EvalPrint(renderer_commands->queue_ptr);
# endif 
#endif
}

void Renderer_SDL_execute(SDL_Renderer *sdl_renderer, 
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

