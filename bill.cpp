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

struct HTexture
{
    U16 w;
    U16 h;
    U32 *bitmap;

    B8 blending;
};

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

HTexture createTextureHandler(U8 *bmp)
{
    BMPHeader *bmpHeader = (BMPHeader*)bmp;
    BMPInfo *bmpInfo = (BMPInfo*)(bmp + sizeof(BMPHeader));
    Assert(bmpInfo->bitcount == 32); // NOTE(annad): RGBA ever!
    HTexture himg = {};
    himg.w = (U16)bmpInfo->width;
    himg.h = (U16)bmpInfo->height;
    himg.bitmap = (U32*)(bmp + bmpHeader->offset);

    return himg;
}

void screenDisplayTexture(Screen *screen, HTexture *img, P2DS32 p)
{
    for (S32 i = 0; i < img->h; i += 1)
    {
        for (S32 j = 0; j < img->w; j += 1)
        {
            // NOTE(annad): Upscale?
            S32 x = (S32)(((F32)j / (F32)img->w) * screen->w);
            S32 y = (S32)(((F32)i / (F32)img->h) * screen->h);
            //S32 x = j;
            //S32 y = i;
            S32 bufidx = (p.y+y)*screen->w+(p.x+x);
            if (bufidx >= screen->h*screen->w || bufidx < 0)
            {
                break;
            }

            S32 tex = img->bitmap[i*img->w+j];
            if (img->blending)
            {
                U8 sa = tex >> 24 & 0xff;
                U8 sr = tex >> 16 & 0xff;
                U8 sg = tex >> 8 & 0xff;
                U8 sb = tex & 0xff;
                // x(t)=A-tA+tB | *255
                // 255*x(T)=255*A-TA+TB
                U8 dr = (screen->buf[i] >> 16 & 0xff);
                U8 dg = (screen->buf[i] >> 8 & 0xff);
                U8 db = (screen->buf[i] & 0xff);
                // 0xBBGGRRAA
                U32 r = (255*dr - sa*dr + sa*sr)/255;
                U32 g = (255*dg - sa*dg + sa*sg)/255;
                U32 b = (255*db - sa*db + sa*sb)/255;

                tex = 0xff << 24 | r << 16 | g << 8 | b;
            }

            screen->buf[bufidx] = tex;
        }
    }
}

internal void gtick(GameIO *io)
{
    // NOTE(annad): Platform layer
    GameStorage *storage = io->storage;
    GameState *gstate = (GameState*)storage->permanent;
    dbg_GameState = gstate;
    RendererHandle *hRenderer = io->hRenderer;
    Screen *screen = io->screen;
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
        gstate->radius = 0.0105f * (F32)(hRenderer->wScreen);
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
        table->w = (S32)(0.8f * (F32)hRenderer->wScreen);
        table->h = (S32)(0.7f * (F32)hRenderer->hScreen);
        table->pos.x = (hRenderer->wScreen - table->w - 2 * (S32)gstate->radius);
        table->pos.y = (hRenderer->hScreen - table->h - 2 * (S32)gstate->radius);
        U8 *tableBitmap = ((U8*)storage->assets + (size_t)ASSETS_BUNDLE_TABLE_BMP);
        table->img = createTextureHandler(tableBitmap);

        V2DF32 screenv = {
            (F32)hRenderer->wScreen,
            (F32)hRenderer->hScreen
        };

        V2DF32 kTable[6][4] = {
            { { 0.242313f, 0.317708f }, { 0.251098f, 0.333333f }, { 0.559297f, 0.333333f }, { 0.562225f, 0.316406f } },
            { { 0.598097f, 0.319010f }, { 0.600000f, 0.333333f }, { 0.907760f, 0.333333f }, { 0.917277f, 0.316406f } },
            { { 0.945827f, 0.363281f }, { 0.937042f, 0.377604f }, { 0.937042f, 0.851562f }, { 0.946559f, 0.867188f } },
            { { 0.918741f, 0.914062f }, { 0.909224f, 0.897135f }, { 0.600293f, 0.897135f }, { 0.598829f, 0.912760f } },
            { { 0.561493f, 0.914062f }, { 0.559297f, 0.897135f }, { 0.251098f, 0.897135f }, { 0.243777f, 0.910156f } },
            { { 0.214495f, 0.865885f }, { 0.223280f, 0.851562f }, { 0.223280f, 0.377604f }, { 0.214495f, 0.364583f } },
        };

        for (U16 i = 0; i < 6; i += 1)
        {
            table->boards[i].p[0] = kTable[i][0] * screenv;
            table->boards[i].p[1] = kTable[i][1] * screenv;
            table->boards[i].p[2] = kTable[i][2] * screenv;
            table->boards[i].p[3] = kTable[i][3] * screenv;
        }
    
        //
        // Balls
        //
        ballsInit(&gstate->table, balls, gstate->radius, 0.591667f, 0.718518f);
        balls[CUE_BALL].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_CUE_BALL_BMP));
        balls[BALL_1].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_1_BMP));
        balls[BALL_2].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_2_BMP));
        balls[BALL_3].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_3_BMP));
        balls[BALL_4].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_4_BMP));
        balls[BALL_5].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_5_BMP));
        balls[BALL_6].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_6_BMP));
        balls[BALL_7].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_7_BMP));
        balls[BALL_8].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_EIGHT_BALL_BMP));
        balls[BALL_9].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_9_BMP));
        balls[BALL_10].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_10_BMP));
        balls[BALL_11].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_11_BMP));
        balls[BALL_12].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_12_BMP));
        balls[BALL_13].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_13_BMP));
        balls[BALL_14].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_14_BMP));
        balls[BALL_15].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_15_BMP));

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

    for (S32 i = 0; i < screen->h * screen->w; i += 1)
    {
        screen->buf[i] = 0xffffffff;
    }

    HTexture test = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_14_BMP));
    // NOTE(annad): test alpha channel
    //HTexture test = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_TEST_ALPHA_BMP));
    test.blending = true;
    for (S32 i = 0; i < screen->h * screen->w; i += 1)
    {
        screen->buf[i] = 0xffff;
    }

    P2DS32 mouse = {
        devices->mouseX,
        devices->mouseY
    };

    localv P2DS32 scalev = {};

    if (devices->dwheel != 0)
    {
        if (devices->keybBtns[KEYB_BTN_LSHIFT])
        {
            scalev.x += 100 * devices->dwheel;
        }
        else
        {
            scalev.y += 100 * devices->dwheel;
        }
    }

    printf("%d %d\n", scalev.x, scalev.y);
    for (S32 i = 0; i < screen->h; i += 1)
    {
        for (S32 j = 0; j < screen->w; j += 1)
        {
            V2DF32 UV = {
                (F32)j / (F32)screen->w,
                (F32)i / (F32)screen->h,
            };
            
            V2DS32 p = {
                (S32)(UV.x * (F32)(test.w + scalev.x)),
                (S32)(UV.y * (F32)(test.h + scalev.y))
            };
            
            if (p.x < test.w && p.y < test.h && p.x > 0 && p.y > 0)
            {
                screen->buf[i*screen->w+j] = test.bitmap[p.y*test.w+p.x];
            }
        }
    }

    //screenDisplayTexture(screen, &test, mouse);

#if 0
    F32 radius = gstate->radius;
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
        ballsInit(&gstate->table, balls, gstate->radius, 0.591667f, 0.718518f);
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
            printf("========================\n");
            Entity *cueball = &balls[CUE_BALL];
            cueball->v = {
                (F32)(cuestick->clipos.x - devices->mouseX),
                (F32)(cuestick->clipos.y - devices->mouseY),
            };

            cuestick->click = false;
        }
    }

    CollideEvent colevent = {};
    while (collideEventPoll(gstate, &colevent))
    {
        switch(colevent.type)
        {
            case COLLIDE_BALL_WALL:
            {
                Entity *e = &balls[colevent.eid];
                V2DF32 nvecwall = colevent.custom.v2df32;
                EvalPrint(e->v.x);
                EvalPrint(e->v.y);
                e->v -= nvecwall * 2.0f * e->v.inner(nvecwall);
#if BILL_CFG_DEV_MODE
                DbgPrint("[COLLIDE] >Solve, ball-wall (eid %d, dt %f)", e->id, colevent.dtBefore);
            } break;
#endif

            case COLLIDE_BALL_BALL:
            {
                Entity *a = &balls[colevent.eid];
                Entity *b = &balls[colevent.custom.s32];
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
#if !BILL_CFG_DEV_MODE
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
            Renderer_pushCmd(hRenderer, RCMD_DRAW_BMP, 
                (S32)(e->p.x - radius), (S32)(e->p.y - radius), (S32)(2.0f * radius), (S32)(2.0f * radius), 
                e->img.bitmap, e->img.width, e->img.height);
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
            V2DF32 nvecwall = {};
            B8 isCollide = ballCheckWallCollide(_e, radius, a, b, &nvecwall);
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
#endif
}

