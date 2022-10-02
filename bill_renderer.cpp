/* 
Author: github.com/annadostoevskaya
File: bill_renderer.cpp
Date: September 29th 2022 9:13 pm 

Description: <empty>
*/

#include "bill_renderer.h"

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
    
#if defined(_BILL_RENDERER_DEBUG_MODE)
# if defined(_DEVELOPER_MODE)
    printf("[PUSH] ");
    EvalPrint(renderer_commands->peak_ptr);
# endif
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
    
#if defined(_BILL_RENDERER_DEBUG_MODE)
# if defined(_DEVELOPER_MODE)
    printf("[PUSH] ");
    EvalPrint(renderer_commands->peak_ptr);
# endif
#endif
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

