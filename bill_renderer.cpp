/* 
Author: github.com/annadostoevskaya
File: bill_renderer.cpp
Date: September 29th 2022 9:13 pm 

Description: <empty>
*/

#include "bill_renderer.h"

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

void renderer_draw_line(Renderer *renderer, 
                        S32 *x1, S32 *y1, S32 *x2, S32 *y2)
{
    RendererCommands *renderer_commands = &renderer->commands;
    // NOTE(annad): Out of memory!
    Assert(renderer_commands->size > renderer_commands->peak_ptr + 
           (4 * sizeof(S32)) +
           sizeof(Renderer_Command));
    
    renderer_commands_insert_command_in_queue(renderer_commands, RENDERER_COMMAND_DRAW_LINE);
    renderer_commands->peak_ptr += sizeof(Renderer_Command);
    
    S32 *args_ptr = (S32*)(renderer_commands_get_current_peak_ptr(renderer_commands));
    args_ptr[0] = *x1;
    args_ptr[1] = *y1;
    args_ptr[2] = *x2;
    args_ptr[3] = *y2;
    renderer_commands->peak_ptr += (4 * sizeof(S32));
    
    LogRendererPush("RENDERER_COMMAND_DRAW_LINE", renderer_commands->peak_ptr);
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

void renderer_push_command(Renderer *renderer, 
                           Renderer_Command command, ...)
{
    va_list argptr;
    va_start(argptr, command);
    
    switch(command)
    {
        case RENDERER_COMMAND_DRAW_FILL_RECT:
        {
            Rect *rect = va_arg(argptr, Rect*);
            renderer_draw_fill_rect(renderer, rect);
            break;
        }
        
        case RENDERER_COMMAND_SET_RENDER_COLOR:
        {
            RGBA_U8 *color = va_arg(argptr, RGBA_U8*);
            renderer_set_renderer_draw_color(renderer, color);
            break;
        }
        
        case RENDERER_COMMAND_DRAW_POINT:
        {
            S32 x = va_arg(argptr, S32);
            S32 y = va_arg(argptr, S32);
            renderer_draw_point(renderer, &x, &y);
            break;
        }
        
        case RENDERER_COMMAND_DRAW_LINE:
        {
            S32 x1 = va_arg(argptr, S32);
            S32 y1 = va_arg(argptr, S32);
            S32 x2 = va_arg(argptr, S32);
            S32 y2 = va_arg(argptr, S32);
            renderer_draw_line(renderer, &x1, &y1, &x2, &y2);
            break;
        }
        
        case RENDERER_COMMAND_DRAW_FILL_CIRCLE:
        {
            // NOTE(annad): Not implement!
            Assert(false);
            break;
        }
        
        case RENDERER_COMMAND_DRAW_CIRCLE:
        {
            S32 x = va_arg(argptr, S32);
            S32 y = va_arg(argptr, S32);
            S32 r = va_arg(argptr, S32);
            renderer_draw_circle(renderer, &x, &y, &r);
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

