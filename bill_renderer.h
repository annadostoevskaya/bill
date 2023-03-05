/* 
Author: github.com/annadostoevskaya
File: bill_renderer.h
Date: September 29th 2022 9:13 pm 

Description: <empty>
*/

#ifndef BILL_RENDERER_H
#define BILL_RENDERER_H

#define RCMD_BUFFER_SIZE BYTE(0x400)

enum Renderer_Command : U8
{
    RCMD_NULL = 0,
    RCMD_SET_RENDER_COLOR,
    RCMD_DRAW_LINE,
    RCMD_DRAW_POINT,
    RCMD_DRAW_CIRCLE,
    RCMD_DRAW_RECT,

    RCMD_DRAW_BMP,

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

#define RENDERER_PUSH_ARG(ARG_PTR, VAR, TYPE, BYTE_COUNTER) \
    do { \
        *((TYPE*)ARG_PTR) = VAR; \
        ARG_PTR += sizeof(TYPE); \
        BYTE_COUNTER += sizeof(TYPE); \
    } while(0)

#define RENDERER_POP_ARG(ARG_PTR, VAR, TYPE, BYTE_COUNTER) \
    do \
    { \
        VAR = *((TYPE*)ARG_PTR); \
        ARG_PTR += sizeof(TYPE); \
        BYTE_COUNTER += sizeof(TYPE); \
    } while(0)

#endif // BILL_RENDERER_H
