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
#endif 

#include "core/mmath.h"
#include "core/memory.h"
#include "core/memory_void.cpp"
#include "core/memory.cpp"
#include "bill_renderer.cpp"
#include "bill_math.h"
#include "bill.h"

#if BILL_CFG_DEV_MODE
#include "bill_debug.h"
#endif 

#define BALL_FRICTION 0.5f
#define BALL_RADIUS 10.0f

inline V2DF32 ballCalcAcc(Entity *ball)
{
    return ball->v * -BALL_FRICTION;
}

internal void ballsInitRack(Entity *balls, F32 x, F32 y)
{
    Assert(BALL_COUNT == 16); // TODO(annad):  5! = 16 and 
    // inverse of factorial is 
    // https://math.stackexchange.com/questions/2078997/inverse-of-a-factorial
    S32 dy = 5;
    S32 dx = 5;
    S32 ballIdx = BALL_2;
    for (S32 i = dx; i > 0; i -= 1)
    {
        dy = i;
        F32 shift = ((5.0f - (F32)i) * BALL_RADIUS);
        for (S32 j = dy; j > 0; j -= 1)
        {
            Assert(ballIdx < BALL_COUNT);
            balls[ballIdx].id = (EntityID)ballIdx;
            balls[ballIdx].p.y = (y + shift + (F32)j * (2.0f * (BALL_RADIUS)));
            // TODO(annad): METRIX, keep going later!
            balls[ballIdx].p.x = (x + (F32)i * (2.0f * (BALL_RADIUS))) + (2.0f * BALL_RADIUS * 10.0f);
            balls[ballIdx].v.x = 0.0f;
            balls[ballIdx].v.y = 0.0f;
            balls[ballIdx].isInit = true;
            balls[ballIdx].isUpdated = false;
            balls[ballIdx].dtUpdate = 0.0f;
            ballIdx += 1;
        }
    }

    balls[CUE_BALL].id = CUE_BALL;
    balls[CUE_BALL].p.x = balls[BALL_16].p.x - (2.0f * BALL_RADIUS * 10.0f);
    balls[CUE_BALL].p.y = balls[BALL_16].p.y;
    balls[CUE_BALL].v.x = 0.0f;
    balls[CUE_BALL].v.y = 0.0f;
    balls[CUE_BALL].isInit = true;
    balls[CUE_BALL].isUpdated = false;
    balls[CUE_BALL].dtUpdate = 0.0f;
}

internal void ballsInit(Rect *table, Entity *balls, F32 x, F32 y)
{
    F32 rackPosX = (x * (F32)table->w);
    F32 rackPosY = (y * (F32)table->h);
    ballsInitRack(balls, rackPosX, rackPosY);
}

internal Entity ballUpdate(Entity *ball, F32 dt)
{
    V2DF32 a = ballCalcAcc(ball);
    Entity updated = *ball;
    updated.p += (a * 0.5f * f32Square(dt) + ball->v * dt);
    updated.v += a * dt;
    return updated;   
}

internal B8 ballCheckTableBoardCollide(Entity *ball, Rect *table, V2DF32 *nvecwall)
{
    if (ball->p.x >= table->x + table->w - BALL_RADIUS)
    {
        nvecwall->x = -1.0f;
        nvecwall->y =  0.0f;
        return true;
    }

    if (ball->p.x <= table->x + BALL_RADIUS)
    {
        nvecwall->x = 1.0f;
        nvecwall->y = 0.0f;
        return true;
    }

    if (ball->p.y <= table->y + BALL_RADIUS)
    {
        nvecwall->x =  0.0f;
        nvecwall->y = -1.0f;
        return true;
    }

    if (ball->p.y >= table->y + table->h - BALL_RADIUS)
    {
        nvecwall->x = 0.0f;
        nvecwall->y = 1.0f;
        return true;
    }

    return false;
}

internal B8 ballCheckBallCollide(Entity *a, Entity *b)
{
    F32 d = (a->p - b->p).getLength();
    return d < (2.0f * BALL_RADIUS);
}

internal F32 ballTimeBeforeWallCollide(Entity *ballA, Rect *table, V2DF32 *nvecwall)
{
    (void)ballA;
    (void)table;
    (void)nvecwall;
    // TODO(annad): Now, is maximum priority level! 
    return 0.0f;
}

internal F32 ballTimeBeforeBallCollide(Entity *ballA, Entity *ballB)
{
    // NOTE(annad): Last update, <date>
    V2DF32 d = ballB->p - ballA->p;
    F32 dl = d.getLength();
    // NOTE(annad): Block with corner cases!
    if (ballCheckBallCollide(ballA, ballB))
    {
        // NOTE(annad): Already collide!
#if BILL_CFG_DEV_MODE
        DbgPrint("[INFO] Already collide%s", "!");
#endif
        return 0.0f;
    }

    F32 v = ballA->v.getLength(); 
    if (f32EpsCompare(v, 0.0f, 0.01f)) // TODO(annad): kowalski analysis.
    {                                  // is this at all possible?
        // NOTE(annad): Never collide!
        // Assert(false);
        return f32Infinity();
    }

    F32 cos = ballA->v.inner(d) / (dl * v);
    if (f32EpsCompare(cos, 0.0f, 0.001f))
    {
        // TODO(annad): Check this later
        // if one of the values goes to zero we get this case
        // Assert(false);
        return 0.0f;
    }

    // NOTE(annad): Calculate distance between collide points
    // 
    // s(fi) = 2r * (1 / cos(w)) * sin( arcsin(2r*S*(1/cos(w))) + w ) where 
    // w - angle berween distance B - A and Velocity vector
    // r  - ball's radius
    // S  - |B - A|
    //
    // See https://www.geogebra.org/m/qqy3e5q9 for more info.
    F32 s = 0.0f;
    if (f32EpsCompare(cos, 1.0f, 0.001f))
    {
        s = dl - (2.0f * BALL_RADIUS);
    }
    else
    {
        F32 aAngle = f32ArcCos(cos);
        F32 A = 2.0f * BALL_RADIUS;
        F32 B = dl;
        F32 sinA = f32Sin(aAngle);
        F32 sinB = (sinA / A) * B;

        // TODO(annad): kowalski analysis.
        // Can this be done better?
        sinB = sinB > 1.0f  ?  1.0f : sinB;
        sinB = sinB < -1.0f ? -1.0f : sinB;

        F32 sinC = f32Sin(f32ArcSin(sinB) - aAngle);
        F32 C = (A / sinA) * sinC;
        s = C;
        if (f32EpsCompare(s, 0.0f, 0.0001f))
        {
            return 0.0f;
        }
    }

    F32 a = 0.5f * ballCalcAcc(ballA).getLength();
    F32 D = f32Sqrt(f32Square(v) - 4.0f * a * s);
    Assert(D == D); // TODO(annad): If to f32Sqrt pass x <= 0
    F32 t = (v - D) / (2.0f * a);
    return t;
}

#if 0
internal void ballSolveCollide2Ball(Entity *a, Entity *b, Entity *c)
{
#if BILL_CFG_DEV_MODE
    DbgPrint("SolveCollide2%s", "\n");
#endif
    // TODO(annad): Rewrite the code in terms of physics
    F32 v = a->v.getLength();
    if(!f32EpsCompare(v, 0.0f, 0.0001f))
    {
        V2DF32 BA = b->p - a->p;
        V2DF32 CA = c->p - a->p;
        F32 lBA = BA.getLength();
        F32 lCA = CA.getLength();
        V2DF32 directA = { a->v.x / v, a->v.y / v };
        V2DF32 directB = { BA.x / lBA, BA.y / lBA };
        V2DF32 directC = { CA.x / lCA, CA.y / lCA };
        F32 cosB = a->v.inner(directB) / (v * directB.getLength());
        F32 cosC = a->v.inner(directC) / (v * directC.getLength());
        F32 vb = (2.0f * v * cosB) / (1.0f + 2.0f * f32Square(cosB));
        F32 vc = (2.0f * v * cosC) / (1.0f + 2.0f * f32Square(cosC));
        b->v += directB * vb;
        c->v += directC * vc;
        a->v = directA * (v - (2.0f * vb * cosB));
    }
}
#endif

internal void ballSolveCollideOneBall(Entity *a, Entity *b)
{
    F32 v = a->v.getLength();
    if(!f32EpsCompare(v, 0.0f, 0.0001f))
    {
        V2DF32 d = b->p - a->p;
        F32 dl = d.getLength();
        V2DF32 directB = { d.x / dl, d.y / dl };
        V2DF32 directA = { directB.y, -directB.x };
        F32 cosA = a->v.inner(directA) / (v * directA.getLength());
        F32 cosB = a->v.inner(directB) / (v * directB.getLength());
        a->v = directA * v * cosA;
        b->v += directB * v * cosB;
    }
}

internal void eventQueueClear(CollideEventQueue *cequeue)
{
    cequeue->pointer = 0;
}

internal S32 eventQueuePeek(CollideEventQueue *cequeue)
{
    F32 minTime = f32Infinity();
    S32 idx = -1;
    CollideEvent *event;
    for (S32 i = 0; i < cequeue->pointer; i += 1)
    {
        event = &(cequeue->pool[i]);
        if (event->dtBefore < minTime && event->dtBefore >= 0.0f)
        {
            idx = i;
            minTime = event->dtBefore;
        }
    }

    return idx;
}

internal void eventQueuePush(CollideEventQueue *cequeue, CollideEvent *event)
{
    Assert(cequeue->count > cequeue->pointer);
    cequeue->pool[cequeue->pointer] = *event;
    cequeue->pointer += 1;
}

internal B8
collideEventPoll(GameState *gstate, CollideEvent *colevent)
{
    CollideEventQueue *queue = &gstate->cequeue;
    eventQueueClear(queue);
    Entity updated = {};
    Rect *table = &gstate->table;
    Entity *balls = (Entity*)(&gstate->balls);
    CollideEvent e = {};
    for (S32 i = 0; i < BALL_COUNT; i += 1)
    {
        Entity *b = &balls[i];
        if (b->isInit && !b->isUpdated)
        {
            updated = ballUpdate(b, b->dtUpdate);
            // NOTE(annad): Out of memory!
            StaticAssert(sizeof(V2DF32) <= sizeof(e.custom));
            V2DF32 *nvecwall = (V2DF32*)(e.custom);
            if (ballCheckTableBoardCollide(&updated, table, nvecwall))
            {
#if BILL_CFG_DEV_MODE
                DbgPrint("[COLLIDE] >Detected, ball-wall (eid %d, dt %f)", b->id, e.dtBefore);
#endif
                e.eid = b->id;
                e.type = COLLIDE_BALL_WALL;
                e.dtBefore = ballTimeBeforeWallCollide(b, table, nvecwall);
                eventQueuePush(queue, &e);
            }
        }
    }

    for (S32 i = 0; i < BALL_COUNT; i += 1)
    {
        Entity *a = &balls[i];
        if (!a->isInit || a->isUpdated) continue;
        updated = ballUpdate(a, a->dtUpdate);
        for (S32 j = 0; j < BALL_COUNT; j += 1)
        {
            if (i == j) continue;
            Entity *b = &balls[j];
            if (!b->isInit) continue;
            if (ballCheckBallCollide(&updated, b))
            {
                e.eid = a->id;
                e.type = COLLIDE_BALL_BALL;
                e.dtBefore = ballTimeBeforeBallCollide(a, b);
                S32 *ballbeid = (S32*)e.custom;
                *ballbeid = b->id;
                eventQueuePush(queue, &e);
#if BILL_CFG_DEV_MODE
                DbgPrint("[COLLIDE] >Detected, ball-ball (a_eid %d, b_eid %d, dt %f)", a->id, b->id, e.dtBefore);
#endif
            }
        }
    }

    S32 eventidx = eventQueuePeek(queue);
    if (eventidx == -1)
    {
        return false;
    }
    
    *colevent = queue->pool[eventidx];
    return true;
}

#if BILL_CFG_DEV_MODE
#include "bill_debug.cpp"
#endif 

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
        // Arena
        //
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
#define TBL_HW_INDEX 0.5185f
#define TBL_WIDTH_F32 0.75f
        S32 tblWidth = (S32)(TBL_WIDTH_F32 * (F32)hRenderer->wScreen);
        S32 tblHeight = (S32)(TBL_HW_INDEX * tblWidth);
        S32 tblXPos = (hRenderer->wScreen - tblWidth - 10);
        S32 tblYPos = (hRenderer->hScreen - tblHeight - 10);
        gstate->table = {
            tblXPos, tblYPos, 
            tblWidth, tblHeight
        };

        //
        // Balls
        //
        ballsInit(&gstate->table, balls, 0.0f, 0.0f);

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
        ballsInit(&gstate->table, balls, 0.75f, 0.5f);
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
            Renderer_pushCmd(hRenderer, RCMD_DRAW_CIRCLE, (S32)e->p.x, (S32)e->p.y, (S32)BALL_RADIUS);
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
    Renderer_pushCmd(hRenderer, RCMD_DRAW_RECT, gstate->table.x, gstate->table.y, gstate->table.w, gstate->table.h);

    Renderer_pushCmd(hRenderer, RCMD_NULL);
}

