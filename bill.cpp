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
#pragma warning(disable: 4189)
# pragma warning(disable: 4100)
# pragma warning(disable: 4702)
#endif 

#include "core/mmath.h"
#include "core/memory.h"
#include "core/memory_void.cpp"
#include "core/memory.cpp"
#include "bill_bitmap.h"
#include "bill_renderer_software.h"

#include "bill.h"
#if BILL_CFG_DEV_MODE
#include "bill_debug.h"
#endif 
#include "bill_ball.cpp"
#include "bill_colevent.cpp"
#include "bill_assets.h"

#include "dev/p2df32_pull.cpp"

void debug_draw_bcurve(Screen *s, V2DF32 p1, V2DF32 p2, V2DF32 p3)
{
    for (F32 t = 0.0f; t < 1.0f; t += 0.001f)
    {
        V2DF32 p = (p1*(1.0f-t) + p2*t)*(1.0f-t) + (p2*(1.0f-t)+p3*t)*t;
        S32 x = (S32)p.x;
        S32 y = (S32)p.y;
        s->buf[y*s->w+x] = 0xffffffff;
    }
}

internal void gtick(GameIO *io)
{
    // NOTE(annad): Platform layer
    GameStorage *storage = io->storage;
    GameState *gstate = (GameState*)storage->permanent;
    dbg_GameState = gstate;
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
        gstate->radius = 0.0105f * (F32)(screen->w);
        // Arena
        // TODO(annad): Memory aligment???
        M_BaseMemory *mVtbl = m_void_base_memory(storage->persistent, storage->persistSize);
        gstate->arena = m_make_arena_reserve(mVtbl, storage->persistSize);
        Assert(gstate->arena.memory != NULL);
        //
        // Table
        //
        table->w = (S32)(0.8f * (F32)screen->w);
        table->h = (S32)(0.7f * (F32)screen->h);
        table->pos.x = (screen->w - table->w - 2 * (S32)gstate->radius);
        table->pos.y = (screen->h - table->h - 2 * (S32)gstate->radius);
        U8 *tableBitmap = ((U8*)storage->assets + (size_t)ASSETS_BUNDLE_TEST_ALPHA_BMP);
        table->img = createTextureHandler(tableBitmap);

        V2DF32 screenv = {
            (F32)screen->w,
            (F32)screen->h
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
        balls[BALL_8].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_EIGHT_BALL_BMP));
        balls[BALL_1].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_1_BMP));
        balls[BALL_2].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_2_BMP));
        balls[BALL_3].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_3_BMP));
        balls[BALL_4].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_4_BMP));
        balls[BALL_5].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_5_BMP));
        balls[BALL_6].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_6_BMP));
        balls[BALL_7].img = createTextureHandler(((U8*)storage->assets + (size_t)ASSETS_BUNDLE_BALL_7_BMP));
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
#if 0
    localv V2DF32 scalev = {1.0f, 1.0f};
    if (devices->dwheel != 0)
    {
        scalev.x += 0.1f * (F32)devices->dwheel;
        scalev.y += 0.1f * (F32)devices->dwheel;
    }
    
    localv V2DF32 position = {};
    localv V2DF32 clickpos = {};
    localv V2DF32 delta = {};
    if (!devices->mouseBtns[MOUSE_BTN_LEFT])
    {
        position += delta;
        delta = {};
        clickpos = {};
    }

    if (devices->mouseBtns[MOUSE_BTN_LEFT])
    {
        if (clickpos.getLength() == 0.0f)
        {
            clickpos = {
                (F32)devices->mouseX / (F32)screen->w,
                (F32)devices->mouseY / (F32)screen->h
            };
        }
        
        delta = {
            clickpos.x - ((F32)devices->mouseX / (F32)screen->w),
            clickpos.y - ((F32)devices->mouseY / (F32)screen->h)
        };

        //printf("x: %f, y: %f\n", position.x + delta.x, position.y + delta.y);
        //printf("w: %f, h: %f\n", scalev.x, scalev.y);
    }
    
    localv S32 state = 0;
    HTexture *test = &table->img;
    textureRender(screen, test, position + delta, scalev); 
#endif

#if 1
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
    
    textureRender(screen, &table->img, 
        V2DF32{(F32)table->pos.x / (F32)screen->w, (F32)table->pos.y/(F32)screen->h}, 
        V2DF32{(F32)table->w/(F32)screen->w, (F32)table->h/(F32)screen->h}
    );
#if 1
    for (U32 i = 0; i < BALL_COUNT; i += 1)
    {
        Entity *e = &balls[i];
        if (e->isInit) 
        {   
            // TODO(annad): For branch prediction optimizations we must 
            // sort entities by array with initialized entities and uninitialized!
            textureRender(screen, &e->img, 
                V2DF32{(e->p.x - gstate->radius) / (F32)screen->w, (e->p.y - gstate->radius) / (F32)screen->h},
                V2DF32{2.0f * gstate->radius / (F32)screen->w, 2.0f * gstate->radius / (F32)screen->h}
            );
        }
    }
#endif

#if BILL_CFG_DEV_MODE
    Entity *_e = &balls[CUE_BALL];
    for (U32 j = 0; j < sizeof(table->boards) / sizeof(table->boards[0]); j += 1)
    {
        for (U32 i = 0; i < sizeof(table->boards[0]) / sizeof(table->boards[0].p[0]) - 1; i += 1)
        {
            P2DF32 a = table->boards[j].p[i];
            P2DF32 b = table->boards[j].p[i+1];
            U32 c = 0xffffffff;

            V2DF32 nvecwall = {};
            
            B8 isCollide = ballCheckLineCollide(_e, gstate->radius, a, b, &nvecwall);
            if (isCollide)
            {
                c = 0xffff0000;
            }
            
            S32 l = (b - a).getLength();
            for (U32 i = 0; i < l; i += 1)
            {
                S32 x = a.x + (S32)((F32)i * ((F32)(b.x - a.x)/(F32)l));
                S32 y = a.y + (S32)((F32)i * ((F32)(b.y - a.y)/(F32)l));
                screen->buf[y*screen->w+x] = c;
            }

            if (isCollide)
            {
                c = 0xffff0000;
            }
        }
    }

    //////////////////////////////////////////////////
    // B(t) = (1-t)[(1-t)P1+tP2] + t[(1-t)P2+tP3], 0 <= t <= 1
    //
    V2DF32 P1_1{274.000000,262.000000};
    V2DF32 P2_1{309.000000,257.000000};
    V2DF32 P3_1{309.000000,227.000000};
    V2DF32 P1_2{718.000000,228.000000};
    V2DF32 P2_2{746.000000,253.000000};
    V2DF32 P3_2{765.000000,229.000000};
    V2DF32 P1_3{1172.000000,228.000000};
    V2DF32 P2_3{1182.000000,258.000000};
    V2DF32 P3_3{1211.000000,261.000000};
    V2DF32 P1_4{1212.000000,623.000000};
    V2DF32 P2_4{1177.000000,633.000000};
    V2DF32 P3_4{1175.000000,657.000000};
    V2DF32 P1_5{718.000000,660.000000};
    V2DF32 P2_5{742.000000,631.000000};
    V2DF32 P3_5{768.000000,658.000000};
    V2DF32 P1_6{312.000000,654.000000};
    V2DF32 P2_6{308.000000,633.000000};
    V2DF32 P3_6{275.000000,622.000000};
    debug_draw_bcurve(screen, P1_1, P2_1, P3_1);
    debug_draw_bcurve(screen, P1_2, P2_2, P3_2);
    debug_draw_bcurve(screen, P1_3, P2_3, P3_3);
    debug_draw_bcurve(screen, P1_4, P2_4, P3_4);
    debug_draw_bcurve(screen, P1_5, P2_5, P3_5);
    debug_draw_bcurve(screen, P1_6, P2_6, P3_6);

    //////////////////////////////////////////////////
#endif
#endif
}

