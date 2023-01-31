/* 
Author: github.com/annadostoevskaya
File: bill_renderer.h
Date: September 29th 2022 9:13 pm 

Description: <empty>
*/

#ifndef BILL_RENDERER_H
#define BILL_RENDERER_H

#define RCMD_BUFFER_SIZE BYTE(0x200)

// NOTE(annad): 1 byte ruining cache line!
enum Renderer_Command : S32
{
    RCMD_NULL = 0,
    RCMD_SET_RENDER_COLOR,
    RCMD_DRAW_LINE,
    RCMD_DRAW_POINT,
    RCMD_DRAW_CIRCLE,
    // RCMD_DRAW_FILL_RECT,
    // RCMD_DRAW_FILL_CIRCLE,
    
    RCMD_COUNT,
};

struct RendererHandle
{
    void *ctx; // NOTE(annad): Pointer for pass custom data.
    U8 *byteCode;
    S32 peak; // NOTE(annad): U32 vs S32?
    S32 size;
    
    S32 wScreen;
    S32 hScreen;
};

inline void Renderer_insertCmd(RendererHandle *hRenderer, Renderer_Command cmd)
{
    Assert((S32)cmd < (S32)RCMD_COUNT);
    Renderer_Command *pCmd = (Renderer_Command*)(hRenderer->byteCode + hRenderer->peak);
    *pCmd = cmd;
    hRenderer->peak += sizeof(Renderer_Command);
}

#endif // BILL_RENDERER_H
