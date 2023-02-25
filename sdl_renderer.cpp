/* 
Author: github.com/annadostoevskaya
File: sdl_renderer.cpp
Date: September 29th 2022 9:14 pm 

Description: <empty>
*/

#pragma warning(push)
#pragma warning(disable : 4309)

internal S32 SDLRenderer_drawCircle(RendererHandle *hRenderer, U8 *cmdPointer)
{
    // NOTE(annad): Error, out of memory!
    Assert(hRenderer->peak - sizeof(Renderer_Command) - 3 * sizeof(S32) >= 0);
    // NOTE(annad): Error, invalid command code!
    Assert(*cmdPointer == RCMD_DRAW_CIRCLE);
    S32 *args = (S32*)(cmdPointer + sizeof(Renderer_Command));
    S32 centX = args[0];
    S32 centY = args[1];
    S32 r = args[2];
    
    // TODO(annad): Remove this later!
    // Midpoint Circle Algorithm.
    S32 d = 2 * r;
    S32 x = (r - 1);
    S32 y = 0;
    S32 tx = 1; 
    S32 ty = 1;
    S32 err = (tx - d);
    SDL_Renderer *sdlRend = (SDL_Renderer*)(hRenderer->ctx);
    
    while(x >= y)
    {
        SDL_RenderDrawPoint(sdlRend, centX + x, centY - y);
        SDL_RenderDrawPoint(sdlRend, centX + x, centY + y);
        SDL_RenderDrawPoint(sdlRend, centX - x, centY - y);
        SDL_RenderDrawPoint(sdlRend, centX - x, centY + y);
        SDL_RenderDrawPoint(sdlRend, centX + y, centY - x);
        SDL_RenderDrawPoint(sdlRend, centX + y, centY + x);
        SDL_RenderDrawPoint(sdlRend, centX - y, centY - x);
        SDL_RenderDrawPoint(sdlRend, centX - y, centY + x);
        
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

    return sizeof(Renderer_Command) + 3 * sizeof(S32);
}

internal S32 SDLRenderer_drawPoint(RendererHandle *hRenderer, U8 *cmdPointer)
{
    // NOTE(annad): Error, out of memory!
    Assert(hRenderer->peak - sizeof(Renderer_Command) - 2 * sizeof(S32) >= 0);
    // NOTE(annad): Error, invalid command code!
    Assert(*cmdPointer == RCMD_DRAW_POINT);
    S32 *args = (S32*)(cmdPointer + sizeof(Renderer_Command));
    S32 x = args[0];
    S32 y = args[1];
    SDL_RenderDrawPoint((SDL_Renderer*)hRenderer->ctx, x, y);
    return sizeof(Renderer_Command) + 2 * sizeof(S32);
}

internal S32 SDLRenderer_drawLine(RendererHandle *hRenderer, U8 *cmdPointer)
{
    // NOTE(annad): Error, out of memory!
    Assert(hRenderer->peak - sizeof(Renderer_Command) - 4 * sizeof(S32) >= 0);
    // NOTE(annad): Error, invalid command code!
    Assert(*cmdPointer == RCMD_DRAW_LINE);
    S32 *args = (S32*)(cmdPointer + sizeof(Renderer_Command));
    S32 x1 = args[0];
    S32 y1 = args[1];
    S32 x2 = args[2];
    S32 y2 = args[3];
    SDL_RenderDrawLine((SDL_Renderer*)hRenderer->ctx, x1, y1, x2, y2);
    return sizeof(Renderer_Command) + 4 * sizeof(S32);
}

internal S32 SDLRenderer_setDrawColor(RendererHandle *hRenderer, U8 *cmdPointer)
{
    // NOTE(annad): Error, out of memory!
    Assert(hRenderer->peak - sizeof(Renderer_Command) - 4 * sizeof(U8) >= 0);
    // NOTE(annad): Error, invalid command code!
    Assert(*cmdPointer == RCMD_SET_RENDER_COLOR);
    U8 *args = cmdPointer + sizeof(Renderer_Command);
    U8 r = args[0];
    U8 g = args[1];
    U8 b = args[2];
    U8 a = args[3];
    SDL_SetRenderDrawColor((SDL_Renderer*)hRenderer->ctx, r, g, b, a);
    return sizeof(Renderer_Command) + 4 * sizeof(U8);
}

internal S32 SDLRenderer_drawRect(RendererHandle *hRenderer, U8 *cmdPointer)
{
    // NOTE(annad): Error, out of memory!
    Assert(hRenderer->peak - sizeof(Renderer_Command) - 4 * sizeof(S32) >= 0);
    // NOTE(annad): Error, invalid command code!
    Assert(*cmdPointer == RCMD_DRAW_RECT);
    S32 *args = (S32*)(cmdPointer + sizeof(Renderer_Command));
    SDL_Rect rect = {
        args[0], args[1],
        args[2], args[3],
    };
    SDL_RenderDrawRect((SDL_Renderer*)hRenderer->ctx, &rect);
    return sizeof(Renderer_Command) + 4 * sizeof(S32);
}

internal S32 SDLRenderer_drawBmp(RendererHandle *hRenderer, U8 *cmdPointer)
{
    // NOTE(annad): Error, invalid command code!
    Assert(*cmdPointer == RCMD_DRAW_BMP);
    U8 *args = (U8*)(cmdPointer + sizeof(Renderer_Command));
    U32 *bitmap;
    U16 bitmapWidth, bitmapHeight;
    S32 byteCounter = 0;

    SDL_Rect targetFrame = {};
    RENDERER_POP_ARG(args, targetFrame.x,       S32,  byteCounter);
    RENDERER_POP_ARG(args, targetFrame.y,       S32,  byteCounter);
    RENDERER_POP_ARG(args, targetFrame.w,       S32,  byteCounter);
    RENDERER_POP_ARG(args, targetFrame.h,       S32,  byteCounter);
    RENDERER_POP_ARG(args, bitmap,              U32*, byteCounter);
    RENDERER_POP_ARG(args, bitmapWidth,         U16,  byteCounter);
    RENDERER_POP_ARG(args, bitmapHeight,        U16,  byteCounter);

    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void*)bitmap, 
            bitmapWidth, bitmapHeight, 32, 4 * bitmapWidth, 
            0xFF0000, 0x00FF00, 0x0000FF, 0xFF000000);
    SDL_Texture *texture = SDL_CreateTextureFromSurface((SDL_Renderer*)hRenderer->ctx, surface);
    SDL_RenderCopy((SDL_Renderer*)hRenderer->ctx, texture, NULL, &targetFrame);
    // NOTE(annad): Error, out of memory!
    Assert(hRenderer->peak - sizeof(Renderer_Command) - byteCounter >= 0);
    return sizeof(Renderer_Command) + byteCounter;
}

void SDLRenderer_exec(RendererHandle *hRenderer)
{
    Assert(hRenderer->peak < hRenderer->size);
    U8 *cmdPointer = hRenderer->byteCode;
    while (hRenderer->peak > 0)
    {
        Assert(cmdPointer < hRenderer->byteCode + hRenderer->size);
        Assert((S32)(*cmdPointer) < (S32)RCMD_COUNT);
        switch (*cmdPointer)
        {
            case RCMD_NULL:
            {
                hRenderer->peak -= sizeof(Renderer_Command);
                Assert(hRenderer->peak == 0);
                return;
            } break;

            case RCMD_SET_RENDER_COLOR:
            {
                S32 shift = SDLRenderer_setDrawColor(hRenderer, cmdPointer);
                hRenderer->peak -= shift;
                cmdPointer += shift;
            } break;

            case RCMD_DRAW_LINE:
            {
                S32 shift = SDLRenderer_drawLine(hRenderer, cmdPointer);
                hRenderer->peak -= shift;
                cmdPointer += shift; 
            } break;

            case RCMD_DRAW_POINT:
            {
                S32 shift = SDLRenderer_drawPoint(hRenderer, cmdPointer);
                hRenderer->peak -= shift;
                cmdPointer += shift;
            } break;

            case RCMD_DRAW_CIRCLE:
            {
                S32 shift = SDLRenderer_drawCircle(hRenderer, cmdPointer);
                hRenderer->peak -= shift;
                cmdPointer += shift;
            } break;
     
            case RCMD_DRAW_BMP:
            {
                S32 shift = SDLRenderer_drawBmp(hRenderer, cmdPointer);
                hRenderer->peak -= shift;
                cmdPointer += shift;
            } break;

            case RCMD_DRAW_RECT:
            {
                S32 shift = SDLRenderer_drawRect(hRenderer, cmdPointer);
                hRenderer->peak -= shift;
                cmdPointer += shift;
            } break;

            default: 
            {
                // NOTE(annad): Error, undefined behaviour.
                Assert(false);
            } break;
        }
    }
}

#pragma warning(pop)

