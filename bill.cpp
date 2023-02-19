/* 
Author: github.com/annadostoevskaya
File: bill.cpp
Date: September 24th 2022 8:05 pm 

Description: <empty> 
*/

#if 1
# pragma warning(disable: 4505)
# pragma warning(disable: 5189)
# pragma warning(disable: 4127)
# pragma warning(disable: 4189)
# pragma warning(disable: 4100)
# pragma warning(disable: 4702)
#endif 

#include "core/mmath.h"
#include "core/memory.h"
#include "core/memory_void.cpp"
#include "core/memory.cpp"
#include "bill_renderer.cpp"
#include "bill.h"
#if BILL_CFG_DEV_MODE
#include "bill_debug.h"
#endif 
#include "bill_ball.cpp"
#include "bill_colevent.cpp"
#if BILL_CFG_DEV_MODE
#include "bill_debug.cpp"
#endif 

#pragma pack(push, 1)
struct BMPHeader
{
    U16 type; // 2
    U32 size; // 6
    U32 RESERVED; // 10
    U32 offset; // 14 bytes
};

struct BMPInfo
{
    U32 size;
    S32 width;
    S32 height;
    U16 planes; // STUB
    U16 bitcount; // STUB
    U32 compression; // STUB
    U32 imgsize;
    S32 xpxperMeter; // STUB
    S32 ypxperMeter; // STUB
    U32 colorsUsed; // STUB
    U32 colorsImportant; // STUB // 40 
    U32 BGRA[1]; // STUB // 44 bytes
};
#pragma pack(pop)

struct BMP 
{
    BMPHeader   *header;
    BMPInfo     *info;
    U32         *bitmap;
};


internal void gtick(GameIO *io)
{
    // NOTE(annad): Platform layer
    GameStorage *storage = io->storage;
    GameState *gstate = (GameState*)storage->permanent;
    dbg_GameState = gstate;
    RendererHandle *hRenderer = io->hRenderer;
    InputDevices *devices = io->devices;

    // NOTE(annad): Game layer
    Entity *balls = (Entity*)(&gstate->balls);
    CueStick *cuestick = &gstate->cuestick;
    if (gstate->isInit == false)
    {
        //
        // Metrics
        //
#define BALLDIAM_PER_WIDTH 0.021f
        gstate->base = (S32)(BALLDIAM_PER_WIDTH * (F32)hRenderer->wScreen);
        gstate->balldiam = (F32)gstate->base;

        //
        // Arena
        // TODO(annad): Memory aligment???
        M_BaseMemory *mVtbl = m_void_base_memory(storage->persistent, storage->persistSize);
        gstate->arena = m_make_arena_reserve(mVtbl, storage->persistSize);
        Assert(gstate->arena.memory != NULL);

        //
        // Renderer
        //
        hRenderer->size = RCMD_BUFFER_SIZE;
        hRenderer->byteCode = (U8*)m_arena_push(&gstate->arena, hRenderer->size);
        Assert(hRenderer->byteCode != NULL);
        
        //
        // Table
        //
#define TABLE_H_PER_W 0.5185f
        S32 tblwidth = 36 * gstate->base;
        S32 tblheight = (S32)(TABLE_H_PER_W * (F32)tblwidth);
        S32 tblxpos = (hRenderer->wScreen - tblwidth - gstate->base);
        S32 tblypos = (hRenderer->hScreen - tblheight - gstate->base);
        gstate->table = {
            tblxpos, tblypos, 
            tblwidth, tblheight
        };

        gstate->baulkline.a.x = (S32)((F32)(gstate->table.x + gstate->table.w) - (1.0f / 5.0f * (F32)gstate->table.w));
        gstate->baulkline.a.y = (gstate->table.y + gstate->table.h);
        gstate->baulkline.b.x = (S32)((F32)(gstate->table.x + gstate->table.w) - (1.0f / 5.0f * (F32)gstate->table.w));
        gstate->baulkline.b.y = gstate->table.y;

        //
        // Balls
        //
        ballsInit(&gstate->table, balls, gstate->balldiam / 2.0f, 0.75f, 0.5f);

        // 
        // CollideEventQueue
        //
        CollideEventQueue cequeue;
        cequeue.count = COLLIDE_EVENT_QUEUE_COUNT;
        cequeue.pointer = 0;
        cequeue.pool = (CollideEvent*)m_arena_push(&gstate->arena, cequeue.count * sizeof(CollideEvent));
        gstate->cequeue = cequeue;

        gstate->isInit = true;
    }

    BMP img;
    img.header = (BMPHeader*)storage->assets;
    img.info = (BMPInfo*)((U8*)storage->assets + sizeof(BMPHeader));
    img.bitmap = (U32*)((U8*)storage->assets + img.header->offset);

    SDL_Surface *sf = SDL_CreateRGBSurfaceFrom((void*)img.bitmap, 
            img.info->width, img.info->height, 
            img.info->bitcount, img.info->width * (img.info->bitcount / 8), 
            0xFF0000, 0x00FF00, 0x0000FF, 0xFF000000);
            // 0xFF, 0xFF0000, 0xFF00, 0xFF);
    SDL_Texture *tx = SDL_CreateTextureFromSurface((SDL_Renderer*)hRenderer->ctx, sf);
    SDL_RenderCopy((SDL_Renderer*)hRenderer->ctx, tx, NULL, NULL);
    return;
    // https://wiki.libsdl.org/SDL2/SDL_RenderCopy
    // https://wiki.libsdl.org/SDL2/SDL_CreateTextureFromSurface
    // https://wiki.libsdl.org/SDL2/SDL_CreateRGBSurfaceFrom


    F32 radius = gstate->balldiam / 2;
    F32 frametime = (F32)io->tick->dt / 1000.0f;
    for (S32 i = 0; i < BALL_COUNT; i += 1)
    {
        if (balls[i].isInit)
        {
            balls[i].isUpdated = false;
            balls[i].dtUpdate = frametime;
        }
    }

    if (devices->keybBtns[KEYB_BTN_RETURN])
    {
        // Reset game
        ballsInit(&gstate->table, balls, gstate->balldiam / 2.0f, 0.75f, 0.5f);
    }

    if (devices->mouseBtns[MOUSE_BTN_LEFT])
    {
        if (!cuestick->click)
        {
            cuestick->clipos.x = devices->mouseX;
            cuestick->clipos.y = devices->mouseY;
            cuestick->click = true;
        }
    }

    if (!devices->mouseBtns[MOUSE_BTN_LEFT])
    {
        if (cuestick->click)
        {
            Entity *cueball = &balls[CUE_BALL];
            cueball->v = {
                (F32)(cuestick->clipos.x - devices->mouseX),
                (F32)(cuestick->clipos.y - devices->mouseY),
            };

            cuestick->click = false;
        }
    }

#if 1
    CollideEvent colevent = {};
    while (collideEventPoll(gstate, &colevent))
    {
        switch(colevent.type)
        {
            case COLLIDE_BALL_WALL:
            {
                Entity *e = &balls[colevent.eid];
                V2DF32 *nvecwall = (V2DF32*)(colevent.custom);
                e->v -= (*nvecwall) * 2.0f * e->v.inner(*nvecwall);
#if BILL_CFG_DEV_MODE
                DbgPrint("[COLLIDE] >Solve, ball-wall (eid %d, dt %f)", e->id, colevent.dtBefore);
#endif
            } break;

            case COLLIDE_BALL_BALL:
            {
                Entity *a = &balls[colevent.eid];
                Entity *b = &balls[*((S32*)colevent.custom)];
                if (a->v.getLength() <= 10.0f)
                {
                    a->v = {};
                    a->isUpdated = true; // TODO(annad): Idk, where is it really supposed to be?
                }
                else
                {
                    if (f32EpsCompare(colevent.dtBefore, 0.0f, 0.001f))
                    {
                        // TODO(annad): It's not solve problem, just hide here
                        a->isUpdated = true;
                    }
                    else
                    {
                        *a = ballUpdate(a, colevent.dtBefore);
                        a->dtUpdate -= colevent.dtBefore;
                    }

                    ballSolveCollideOneBall(a, b);
                }
#if BILL_CFG_DEV_MODE
                DbgPrint("[COLLIDE] >Solve, ball-ball (a_eid %d, b_eid %d, dt %f)", a->id, b->id, colevent.dtBefore);
#endif
            } break;

            default: 
            {
                // NOTE(annad): Invalid Program Path
                Assert(false);
            } break;
        }
    }

    for (S32 i = 0; i < BALL_COUNT; i += 1)
    {
        Entity *e = &balls[i];
        if (e->isInit && !e->isUpdated)
        {
            *e = ballUpdate(e, e->dtUpdate);
            // e->dtUpdate = 0.0f;
            e->isUpdated = true;
        }
    }
#endif 

    Renderer_pushCmd(hRenderer, RCMD_SET_RENDER_COLOR, 0xff, 0xff, 0xff, 0xff);
    for (S32 i = 0; i < BALL_COUNT; i += 1)
    {
        Entity *e = &balls[i];
        if (e->isInit) // TODO(annad): For branch prediction optimizations we must 
        {              // sort entities by array with initialized entities and uninitialized!
            Renderer_pushCmd(hRenderer, RCMD_DRAW_CIRCLE, (S32)e->p.x, (S32)e->p.y, (S32)radius);
        }
    }

    if (cuestick->click)
    {
        Renderer_pushCmd(hRenderer, RCMD_SET_RENDER_COLOR, 0xff, 0x00, 0x00, 0xff);
        Renderer_pushCmd(hRenderer, RCMD_DRAW_LINE, 
                (S32)balls[CUE_BALL].p.x, 
                (S32)balls[CUE_BALL].p.y,
                (S32)balls[CUE_BALL].p.x + cuestick->clipos.x - devices->mouseX,
                (S32)balls[CUE_BALL].p.y + cuestick->clipos.y - devices->mouseY);
    }

    Renderer_pushCmd(hRenderer, RCMD_SET_RENDER_COLOR, 0xff, 0xff, 0x00, 0xff);
    Renderer_pushCmd(hRenderer, RCMD_DRAW_RECT, 
            gstate->table.x, gstate->table.y, 
            gstate->table.w, gstate->table.h);

    Renderer_pushCmd(hRenderer, RCMD_DRAW_LINE, 
            gstate->baulkline.a.x, gstate->baulkline.a.y,
            gstate->baulkline.b.x, gstate->baulkline.b.y);
    
    Renderer_pushCmd(hRenderer, RCMD_NULL);
}

