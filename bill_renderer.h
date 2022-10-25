/* 
Author: github.com/annadostoevskaya
File: bill_renderer.h
Date: September 29th 2022 9:13 pm 

Description: <empty>
*/

#ifndef BILL_RENDERER_H
#define BILL_RENDERER_H

#include "base_types.h"

// NOTE(annad): Define it for print PUSH & POP operations.
// #define _BILL_RENDERER_DEBUG_MODE 1

// TODO(annad): beautify output, example:
// [PUSH] COMMAND_NAME --------------- 4
// [PUSH] SOME_COMMAND_NAME ---------- 16
// [PUSH] ANOTHER_COMMAND_NAME ------- 24 
#if defined(_DEVELOPER_MODE)
# if defined(_BILL_RENDERER_DEBUG_MODE)
#  define DebugLogRenderer(action, cmd_name, peak_ptr) printf("[" ## action ## "] " ## cmd_name ## ": %lld\n", peak_ptr);
#  define LogRendererPush(cmd_name, peak_ptr) DebugLogRenderer("PUSH", cmd_name, peak_ptr)
#  define LogRendererPop(cmd_name, peak_ptr) DebugLogRenderer("POP", cmd_name, peak_ptr)
# else // _BILL_RENDERER_DEBUG_MODE
#  define LogRendererPush(cmd_name, peak_ptr) 
#  define LogRendererPop(cmd_name, peak_ptr) 
# endif
# else // _DEVELOPER_MODE
#  define DebugLogRenderer(cmd_name, peak_ptr) 
#  define LogRendererPop(cmd_name, peak_ptr) 
#endif

enum Renderer_Command
{
    RENDERER_COMMAND_NULL = 0,
    RENDERER_COMMAND_DRAW_FILL_RECT,
    RENDERER_COMMAND_SET_RENDER_COLOR,
    RENDERER_COMMAND_DRAW_POINT,
    RENDERER_COMMAND_DRAW_LINE,
    RENDERER_COMMAND_DRAW_CIRCLE,
    RENDERER_COMMAND_DRAW_FILL_CIRCLE,
    
    
    RENDERER_COMMAND_COUNT,
};

#define RENDERER_COMMAND_BUFFER_SIZE BYTE(2 * 0xff)

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
