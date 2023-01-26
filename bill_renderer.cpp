/* 
Author: github.com/annadostoevskaya
File: bill_renderer.cpp
Date: September 29th 2022 9:13 pm 

Description: <empty>
*/

#include "bill_renderer.h"

#if _COMPILER_CLANG
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wvarargs"
#endif

/*
void renderer_draw_fill_rect(Renderer *renderer, Rect *rect)
{
    RendererCommands *renderer_commands = &renderer->commands;
    // NOTE(annad): Out of memory!
    Assert(renderer_commands->size > 
           renderer_commands->peak_ptr + 
           sizeof(Renderer_Command) +
           sizeof(Rect));
    
    renderer_commands_insert_command_in_queue(renderer_commands, RENDERER_COMMAND_DRAW_FILL_RECT);
    renderer_commands->peak_ptr += sizeof(Renderer_Command);
    
    S32 *args_ptr = (S32*)(renderer_commands_get_current_peak_ptr(renderer_commands));
    args_ptr[0] = rect->x;
    args_ptr[1] = rect->y;
    args_ptr[2] = rect->w;
    args_ptr[3] = rect->h;
    renderer_commands->peak_ptr += sizeof(Rect);
    
    LogRendererPush("RENDERER_COMMAND_DRAW_FILL_RECT", renderer_commands->peak_ptr);
}

void renderer_set_renderer_draw_color(Renderer *renderer, RGBA_U8 *color)
{
    RendererCommands *renderer_commands = &renderer->commands;
    // NOTE(annad): Out of memory!
    Assert(renderer_commands->size > renderer_commands->peak_ptr + 
           sizeof(RGBA_U8) +
           sizeof(Renderer_Command));
    
    renderer_commands_insert_command_in_queue(renderer_commands, RENDERER_COMMAND_SET_RENDER_COLOR);
    renderer_commands->peak_ptr += sizeof(Renderer_Command);
    
    U8 *args_ptr = (U8*)(renderer_commands_get_current_peak_ptr(renderer_commands));
    args_ptr[0] = color->r;
    args_ptr[1] = color->g;
    args_ptr[2] = color->b;
    args_ptr[3] = color->a;
    renderer_commands->peak_ptr += sizeof(RGBA_U8);
    
    LogRendererPush("RENDERER_COMMAND_SET_RENDER_COLOR", renderer_commands->peak_ptr);
}

void renderer_draw_point(Renderer *renderer, S32 *x, S32 *y)
{
    RendererCommands *renderer_commands = &renderer->commands;
    // NOTE(annad): Out of memory!
    Assert(renderer_commands->size > renderer_commands->peak_ptr + 
           (2 * sizeof(S32)) +
           sizeof(Renderer_Command));
    
    renderer_commands_insert_command_in_queue(renderer_commands, RENDERER_COMMAND_DRAW_POINT);
    renderer_commands->peak_ptr += sizeof(Renderer_Command);
    
    S32 *args_ptr = (S32*)(renderer_commands_get_current_peak_ptr(renderer_commands));
    args_ptr[0] = *x;
    args_ptr[1] = *y;
    renderer_commands->peak_ptr += (2 * sizeof(S32));
    
    LogRendererPush("RENDERER_COMMAND_DRAW_POINT", renderer_commands->peak_ptr);
}


void renderer_draw_circle(Renderer *renderer, S32 *x, S32 *y, S32 *r)
{
    RendererCommands *renderer_commands = &renderer->commands;
    // NOTE(annad): Out of memory!
    Assert(renderer_commands->size > renderer_commands->peak_ptr + 
           (3 * sizeof(S32)) +
           sizeof(Renderer_Command));
    
    renderer_commands_insert_command_in_queue(renderer_commands, RENDERER_COMMAND_DRAW_CIRCLE);
    renderer_commands->peak_ptr += sizeof(Renderer_Command);
    
    S32 *args_ptr = (S32*)(renderer_commands_get_current_peak_ptr(renderer_commands));
    args_ptr[0] = *x;
    args_ptr[1] = *y;
    args_ptr[2] = *r;
    renderer_commands->peak_ptr += (3 * sizeof(S32));
    
    LogRendererPush("RENDERER_COMMAND_DRAW_CIRCLE", renderer_commands->peak_ptr);
}
*/

inline void Renderer_insertCmd(RendererHandle *hRenderer, Renderer_Command cmd)
{
    Assert((S32)cmd < (S32)RCMD_COUNT);
    Renderer_Command *pCmd = (Renderer_Command*)(hRenderer->byteCode + hRenderer->peak);
    *pCmd = cmd;
    hRenderer->peak += sizeof(Renderer_Command);
}

internal void Renderer_drawCircle(RendererHandle *hRenderer, S32 x, S32 y, S32 r)
{
    // NOTE(annad): Error, out of memory!
    Assert(hRenderer->size > hRenderer->peak + sizeof(Renderer_Command) + 3 * sizeof(S32));
    Renderer_insertCmd(hRenderer, RCMD_DRAW_CIRCLE);
    S32 *args = (S32*)(hRenderer->byteCode + hRenderer->peak);
    args[0] = x;
    args[1] = y;
    args[2] = r;
    hRenderer->peak += 3 * sizeof(S32);
}

internal void Renderer_drawPoint(RendererHandle *hRenderer, S32 x, S32 y)
{
    // NOTE(annad): Error, out of memory!
    Assert(hRenderer->size > hRenderer->peak + sizeof(Renderer_Command) + 2 * sizeof(S32));
    Renderer_insertCmd(hRenderer, RCMD_DRAW_POINT);
    S32 *args = (S32*)(hRenderer->byteCode + hRenderer->peak);
    args[0] = x;
    args[1] = y;
    hRenderer->peak += 2 * sizeof(S32);
}

internal void Renderer_drawLine(RendererHandle *hRenderer, S32 x1, S32 y1, S32 x2, S32 y2)
{
    // NOTE(annad): Error, out of memory!
    Assert(hRenderer->size > hRenderer->peak + sizeof(Renderer_Command) + 4 * sizeof(S32));
    Renderer_insertCmd(hRenderer, RCMD_DRAW_LINE);
    S32 *args = (S32*)(hRenderer->byteCode + hRenderer->peak);
    args[0] = x1;
    args[1] = y1;
    args[2] = x2;
    args[3] = y2;
    hRenderer->peak += 4 * sizeof(S32);
}

internal void Renderer_setDrawColor(RendererHandle *hRenderer, U8 r, U8 g, U8 b, U8 a)
{
    // NOTE(annad): Error, out of memory!
    Assert(hRenderer->size > hRenderer->peak + sizeof(Renderer_Command) + 4 * sizeof(U8));
    Renderer_insertCmd(hRenderer, RCMD_SET_RENDER_COLOR);
    U8 *args = hRenderer->byteCode + hRenderer->peak;
    args[0] = r;
    args[1] = g;
    args[2] = b;
    args[3] = a;
    hRenderer->peak += 4 * sizeof(U8);
}

internal void Renderer_pushCmd(RendererHandle *hRenderer, Renderer_Command rcmd, ...)
{
    va_list argptr;
    va_start(argptr, rcmd);

    switch (rcmd)
    {
        case RCMD_NULL:
        {
            // NOTE(annad): Error, is just for internal use command!
            Assert(false);
        } break;

        case RCMD_SET_RENDER_COLOR:
        {
            U8 r = va_arg(argptr, U8);
            U8 g = va_arg(argptr, U8);
            U8 b = va_arg(argptr, U8);
            U8 a = va_arg(argptr, U8);
            Renderer_setDrawColor(hRenderer, r, g, b, a);
        } break;

        case RCMD_DRAW_LINE:
        {
            S32 x1 = va_arg(argptr, S32);
            S32 y1 = va_arg(argptr, S32);
            S32 x2 = va_arg(argptr, S32);
            S32 y2 = va_arg(argptr, S32);
            Renderer_drawLine(hRenderer, x1, y1, x2, y2);
        } break;

        case RCMD_DRAW_POINT:
        {
            S32 x = va_arg(argptr, S32);
            S32 y = va_arg(argptr, S32);
            Renderer_drawPoint(hRenderer, x, y);
        } break;

        case RCMD_DRAW_CIRCLE:
        {
            S32 x = va_arg(argptr, S32);
            S32 y = va_arg(argptr, S32);
            S32 r = va_arg(argptr, S32);
            Renderer_drawCircle(hRenderer, x, y, r);
        } break;

        default: 
        {
            // NOTE(annad): Unknown command code.
            Assert(false);
        } break;
    }

    va_end(argptr);
}

#if _COMPILER_CLANG
# pragma clang diagnostic pop
#endif

