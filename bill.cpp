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
    U16 bitcount;
    U32 compression; // STUB
    U32 imgsize;
    S32 xpxperMeter; // STUB
    S32 ypxperMeter; // STUB
    U32 colorsUsed; // STUB
    U32 colorsImportant; // STUB // 40 
    U32 BGRA[1]; // STUB // 44 bytes
};
#pragma pack(pop)

struct ImageAsset
{
    U16 width;
    U16 height;
    U32 *bitmap;
};

ImageAsset createImageAsset(U8 *bmpAsset)
{
    BMPHeader *bmpHeader = (BMPHeader*)bmpAsset;
    BMPInfo *bmpInfo = (BMPInfo*)(bmpAsset + sizeof(BMPHeader));
    Assert(bmpInfo->bitcount == 32); // NOTE(annad): RGBA ever!
    ImageAsset imgAsset;
    imgAsset.width = (U16)bmpInfo->width;
    imgAsset.height = (U16)bmpInfo->height;
    imgAsset.bitmap = (U32*)(bmpAsset + bmpHeader->offset);

    return imgAsset;
}

#include "bill.h"
#if BILL_CFG_DEV_MODE
#include "bill_debug.h"
#endif 
#include "bill_ball.cpp"
#include "bill_colevent.cpp"
#if BILL_CFG_DEV_MODE
#include "bill_debug.cpp"
#endif 

#include "bill_assets.h"

/*
 * TODO(annad): Here it is necessary to explain how it works!!!
 */ 
internal B8 ballCheckWallCollide(Entity *ball, F32 radius, P2DF32 a, P2DF32 b)
{
    V2DF32 line = b - a;
    V2DF32 nx = line.getNormalize(); 
    V2DF32 ny = {-nx.y, nx.x};
    V2DF32 p = {
        (ball->p - a).inner(nx), 
        (ball->p - a).inner(ny),
    };

    EvalPrintF(p.x);
    EvalPrintF(p.y);
    return (radius >= p.y && 
        p.x >= -radius && p.x <= line.getLength() + radius);
}

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
    Table *table = &gstate->table;
    CueStick *cuestick = &gstate->cuestick;
    if (gstate->isInit == false)
    {
        //
        // Assets
        //
        U8 *assets = (U8*)storage->assets;
        // Metrics
        //
#define BALLDIAM_PER_WIDTH 0.021f // TODO(annad): What the fuck, remove this!!!!
        gstate->base = (BALLDIAM_PER_WIDTH * (F32)hRenderer->wScreen);
        gstate->balldiam = (F32)gstate->base;
        // Arena
        // TODO(annad): Memory aligment???
        M_BaseMemory *mVtbl = m_void_base_memory(storage->persistent, storage->persistSize);
        gstate->arena = m_make_arena_reserve(mVtbl, storage->persistSize);
        Assert(gstate->arena.memory != NULL);
        // Renderer
        //
        hRenderer->size = RCMD_BUFFER_SIZE;
        hRenderer->byteCode = (U8*)m_arena_push(&gstate->arena, hRenderer->size);
        Assert(hRenderer->byteCode != NULL);
        
        //
        // Table
        //
        table->w = 37 * (S32)gstate->base;
        table->h = 19 * (S32)gstate->base;
        table->pos.x = (hRenderer->wScreen - table->w - (S32)gstate->base);
        table->pos.y = (hRenderer->hScreen - table->h - (S32)gstate->base);
        table->collider.x = (S32)((F32)table->pos.x + 1.5f * gstate->base);
        table->collider.y = (S32)((F32)table->pos.y + 1.4f * gstate->base);
        table->collider.w = (S32)((F32)table->w - (1.5f + 1.5f) * gstate->base);
        table->collider.h = (S32)((F32)table->h - (1.4f + 1.4f) * gstate->base);
        U8 *tableBitmap = ((U8*)storage->assets + (size_t)ASSETS_BUNDLE_TABLE_BMP);
        table->img = createImageAsset(tableBitmap);

        V2DF32 kTable[6][4] = {
            { { 2.87f, 1.45f}, { 0.4f,   0.4f}, { 14.15f,  0.0f }, { 0.12f, -0.4f} },
            { { 1.65f, 0.0f }, { 0.12f,  0.4f}, { 14.15f,  0.0f }, { 0.4f,  -0.4f} },
            { { 1.3f,  1.2f }, {-0.4f,   0.4f}, { 0.0f,    12.8f}, { 0.4f,   0.4f} },
            { {-1.3f,  1.2f }, {-0.4f,  -0.4f}, {-14.15f,  0.0f }, {-0.12f,  0.4f} },
            { {-1.65f, 0.0f }, {-0.12f, -0.4f}, {-14.15f,  0.0f }, {-0.4f,   0.4f} },
            { {-1.3f, -1.2f }, { 0.4f,  -0.4f}, { 0.0f,   -12.8f}, {-0.4f,  -0.4f} },
        };

        table->boards[0].p[0] = {
            (F32)table->pos.x + kTable[0][0].x * gstate->base, 
            (F32)table->pos.y + kTable[0][0].y * gstate->base
        };
        table->boards[0].p[1] = table->boards[0].p[0] + kTable[0][1] * gstate->base;
        table->boards[0].p[2] = table->boards[0].p[1] + kTable[0][2] * gstate->base;
        table->boards[0].p[3] = table->boards[0].p[2] + kTable[0][3] * gstate->base;

        for (U16 i = 1; i < 6; i += 1)
        {
            table->boards[i].p[0] = table->boards[i-1].p[3] + kTable[i][0] * gstate->base;
            table->boards[i].p[1] = table->boards[i].p[0]   + kTable[i][1] * gstate->base;
            table->boards[i].p[2] = table->boards[i].p[1]   + kTable[i][2] * gstate->base;
            table->boards[i].p[3] = table->boards[i].p[2]   + kTable[i][3] * gstate->base;
        }

        //
        // Balls
        //
        ballsInit(&gstate->table, balls, gstate->balldiam / 2.0f, 0.626f, 0.71f);

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

    F32 radius = gstate->balldiam / 2.0f;
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
        ballsInit(&gstate->table, balls, gstate->balldiam / 2.0f, 0.626f, 0.71f);
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
#if 0
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
#endif
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
    
    // TODO(annad): srcrect...
    Renderer_pushCmd(hRenderer, RCMD_DRAW_BMP, 
        table->pos.x, table->pos.y, table->w, table->h, 
        table->img.bitmap, table->img.width, table->img.height);
    Renderer_pushCmd(hRenderer, RCMD_SET_RENDER_COLOR, 0xff, 0xff, 0xff, 0xff);
    Renderer_pushCmd(hRenderer, RCMD_DRAW_RECT, table->pos.x, table->pos.y, table->w, table->h);
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
        Renderer_pushCmd(hRenderer, RCMD_SET_RENDER_COLOR, 0xff, 0xff, 0xff, 0xff);
    }

#if BILL_CFG_DEV_MODE
    Entity *_e = &balls[CUE_BALL];
    for (U32 j = 0; j < sizeof(table->boards) / sizeof(table->boards[0]); j += 1)
    {
        for (U32 i = 0; i < sizeof(table->boards[0]) / sizeof(table->boards[0].p[0]) - 1; i += 1)
        {
            P2DF32 a = table->boards[j].p[i];
            P2DF32 b = table->boards[j].p[i+1];
            B8 isCollide = ballCheckWallCollide(_e, radius, a, b);
            if (isCollide)
            {
                Renderer_pushCmd(hRenderer, 
                    RCMD_SET_RENDER_COLOR, 
                    0xff, 0x00, 0x00, 0xff);
            }

            Renderer_pushCmd(hRenderer, RCMD_DRAW_LINE, 
                (S32)a.x, (S32)a.y, 
                (S32)b.x, (S32)b.y);

            if (isCollide)
            {
                Renderer_pushCmd(hRenderer, 
                    RCMD_SET_RENDER_COLOR, 
                    0xff, 0xff, 0xff, 0xff);
            }
        }
    }

    Renderer_pushCmd(hRenderer, RCMD_DRAW_RECT, 
        table->collider.x, table->collider.y, 
        table->collider.w, table->collider.h);
#endif

    Renderer_pushCmd(hRenderer, RCMD_NULL);
}

