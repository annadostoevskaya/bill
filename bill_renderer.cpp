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

void Renderer_drawPoint(Renderer *renderer, S32 *x, S32 *y)
{
    RendererCommands *renderer_commands = &renderer->commands;
    // NOTE(annad): Out of memory!
    Assert(renderer_commands->size > renderer_commands->peak_ptr + 
           (2 * sizeof(S32)) +
           sizeof(Renderer_Command));
    
    RendererCommands_insertCommandInQueue(renderer_commands, RENDERER_COMMAND_DRAW_POINT);
    renderer_commands->peak_ptr += sizeof(Renderer_Command);
    
    S32 *args_ptr = (S32*)(RendererCommands_getCurrentPeakPtr(renderer_commands));
    args_ptr[0] = *x;
    args_ptr[1] = *y;
    renderer_commands->peak_ptr += (2 * sizeof(S32));
    
#if defined(_BILL_RENDERER_DEBUG_MODE)
# if defined(_DEVELOPER_MODE)
    printf("[PUSH] ");
    EvalPrint(renderer_commands->peak_ptr);
# endif
#endif
}

void Renderer_drawLine(Renderer *renderer, 
                       S32 *x1, S32 *y1, S32 *x2, S32 *y2)
{
    RendererCommands *renderer_commands = &renderer->commands;
    // NOTE(annad): Out of memory!
    Assert(renderer_commands->size > renderer_commands->peak_ptr + 
           (4 * sizeof(S32)) +
           sizeof(Renderer_Command));
    
    RendererCommands_insertCommandInQueue(renderer_commands, RENDERER_COMMAND_DRAW_LINE);
    renderer_commands->peak_ptr += sizeof(Renderer_Command);
    
    S32 *args_ptr = (S32*)(RendererCommands_getCurrentPeakPtr(renderer_commands));
    args_ptr[0] = *x1;
    args_ptr[1] = *y1;
    args_ptr[2] = *x2;
    args_ptr[3] = *y2;
    renderer_commands->peak_ptr += (4 * sizeof(S32));
    
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
        
        case RENDERER_COMMAND_DRAW_POINT:
        {
            S32 x = va_arg(argptr, S32);
            S32 y = va_arg(argptr, S32);
            Renderer_drawPoint(renderer, &x, &y);
            break;
        }
        
        case RENDERER_COMMAND_DRAW_LINE:
        {
            S32 x1 = va_arg(argptr, S32);
            S32 y1 = va_arg(argptr, S32);
            S32 x2 = va_arg(argptr, S32);
            S32 y2 = va_arg(argptr, S32);
            Renderer_drawLine(renderer, &x1, &y1, &x2, &y2);
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

