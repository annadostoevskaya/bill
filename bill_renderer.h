/* 
Author: github.com/annadostoevskaya
File: bill_renderer.h
Date: September 29th 2022 9:13 pm 

Description: <empty>
*/

#ifndef BILL_RENDERER_H
#define BILL_RENDERER_H

// NOTE(annad): Define it for print PUSH & POP operations.
// #define _BILL_RENDERER_DEBUG_MODE 1

enum Renderer_Command
{
    RENDERER_COMMAND_NULL = 0,
    RENDERER_COMMAND_DRAW_FILL_RECT,
    RENDERER_COMMAND_SET_RENDER_COLOR,
    
    RENDERER_COMMAND_COUNT,
};

#define RENDERER_COMMAND_BUFFER_SIZE 0xff

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

#endif // BILL_RENDERER_H
