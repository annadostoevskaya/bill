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
#include "_debug.cpp"
#endif 

/*
Ball update_ball(Ball *ball, F32 dt)
{
    // NOTE(annad):
    // x = f(t) = (a/2)t^2 + v0*t + x0;
    // v = x' = f(t)' = at + v0;
    // a = x'' = f(t)'' = a;
    // x0 += (a/2)t^2 + v0*t;
    // v0 += at;
    // TODO(annad): ODE!!
    // TODO(annad): We need to find coef. of 
    // friction of the billiard ball on the table.
    // x''(t) = -N * x'(t), N = m * omega
    Assert(ball->pos.x > -1000);
    Vec2Dim<F32> ball_acc = ball->vel * (-BALL_FRICTION);
    Ball updated_ball = *ball;
    updated_ball.pos += ball_acc * 0.5f * f32Square(dt) + ball->vel * dt;
    updated_ball.vel += ball_acc * dt;
    return updated_ball;
}

// TODO(annad): Refactoring...
B32 balls_is_collide(Ball *ball_a, Ball *ball_b)
{
    F32 distance = (ball_b->pos - ball_a->pos).getLength();
    return distance < (2.0f * BALL_RADIUS);
}

F32 t_before_collide(Ball *ball_a, Ball *ball_b)
{
    Vec2Dim<F32> ball_a_acc = ball_a->vel * (-BALL_FRICTION);
    F32 t = 0.0f;
    const F32 epsilon = 0.001f;
    Vec2Dim<F32> delta_pos = ball_b->pos - ball_a->pos;
    F32 cos_fi = ball_a->vel.innerProduct(delta_pos) 
        / (delta_pos.getLength() * ball_a->vel.getLength());
    if(cos_fi != cos_fi || f32Abs(cos_fi) < 0.001f ) // TODO(annad): is_infinity()?
    {
        // TODO(annad): I don’t quite understand what can be done with this, 
        // but this case means that all the power of the ball has been absorbed.
        t = 0.0f;
    }
    else
    {
        // NOTE(annad): Get distance between collide points
        // s(fi) = 2r * csc(fi) * sin( arcsin(2r*S*csc(fi)) +  fi), 
        // fi - angle between Vec(B - A), Vec(Velocity)
        // r - ball radius
        // S - |B - A|
        F32 s = 0.0f;
        if(f32Abs(cos_fi  - 1.0f) < epsilon)
        {
            s = delta_pos.getLength() - (2.0f * BALL_RADIUS);
        }
        else
        {
            F32 fi_angle = defaultArcCos(cos_fi); // angle delta_pos,v
            F32 side_a = 2.0f * BALL_RADIUS; // 2r
            F32 side_b = delta_pos.getLength(); // |B - A|
            F32 sin_a = defaultSin(fi_angle);
            F32 sin_b = (sin_a / side_a) * side_b;
            // TODO(annad): Sometimes we get sin_c - NaN. wtf?
            if(sin_b > 1.0f) // TODO(annad): Remove this, normalize floating point math
            {
                // EvalPrintF(sin_b);
                sin_b = 1.0f;
            }
            else if(sin_b < -1.0f)
            {
                // EvalPrintF(sin_b);
                sin_b = -1.0f;
            }
            
            F32 sin_c = defaultSin(defaultArcSin(sin_b) - fi_angle); 
            F32 side_c = (side_a / sin_a) * sin_c; // s before collide
            s = side_c;
            // TODO(annad): Collision order!
            // EvalPrintF(s);
            Assert(s == s); // NOTE(annad): S = [2.0f ~ 21.0f], when V -> inf.
            EvalPrintF(s);
            // FORCE_UPDATE();
            // Assert(s > 0.0f); // TODO(annad): When ball on boundary!!! wtf?????
            if(s < 0.0f)
            {
                t = 0.0f;
                return t;
            }
        }
        
        F32 v = ball_a->vel.getLength();
        F32 a = 0.5f * ball_a_acc.getLength();
        // EvalPrintF(a);
        F32 discriminant = (F32)sqrt(f32Square(v) - 4.0f * a * s);
        Assert(discriminant == discriminant);
        // TODO(annad): math, sqrt
        t = (v - discriminant) / (2.0f * a); // NOTE(annad): what is t < 0.0f?
    }
    
    return t;
}

void three_balls_collide_handle(Ball *ball_a, Ball *ball_b, Ball *ball_c)
{
    // __debugbreak();
    // TODO(annad): Rewrite the code in terms of physics
    Vec2Dim<F32> result_vel = ball_a->vel;
    if(result_vel.getLength() == 0)
    {
        return;
    }

    Vec2Dim<F32> delta_pos_ba = ball_b->pos - ball_a->pos; 
    Vec2Dim<F32> delta_pos_ca = ball_c->pos - ball_a->pos;

    Vec2Dim<F32> direct_b = {
        delta_pos_ba.x / delta_pos_ba.getLength(),
        delta_pos_ba.y / delta_pos_ba.getLength()
    };

    Vec2Dim<F32> direct_c = {
        delta_pos_ca.x / delta_pos_ca.getLength(),
        delta_pos_ca.y / delta_pos_ca.getLength()
    };

    Vec2Dim<F32> direct_a = {
        ball_a->vel.x / ball_a->vel.getLength(),
        ball_a->vel.y / ball_a->vel.getLength()
    };

    F32 cos_teta_b = ball_a->vel.innerProduct(direct_b)
        / (ball_a->vel.getLength() * direct_b.getLength());
    F32 cos_teta_c = ball_a->vel.innerProduct(direct_c)
        / (ball_a->vel.getLength() * direct_c.getLength());

    F32 vel_sclr_b = (2.0f * result_vel.getLength() * cos_teta_b) 
        / 1.0f + 2.0f * f32Square(cos_teta_b);
    F32 vel_sclr_c = (2.0f * result_vel.getLength() * cos_teta_c) 
        / 1.0f + 2.0f * f32Square(cos_teta_c);

    ball_b->vel = direct_b * vel_sclr_b;
    ball_c->vel = direct_c * vel_sclr_c;
    ball_a->vel = direct_a * (result_vel.getLength() - (2.0f * vel_sclr_b * cos_teta_b));
}

void two_balls_collide_handle(Ball *ball_a, Ball *ball_b)
{
    Vec2Dim<F32> result_vel = ball_a->vel + ball_b->vel; 
    if(result_vel.getLength() == 0)
    {
        return;
    }
    
    Vec2Dim<F32> delta_pos = ball_b->pos - ball_a->pos;
    // NOTE(annad): Recalculate velocity after collision.
    Vec2Dim<F32> direct_b = {
        delta_pos.x / delta_pos.getLength(),
        delta_pos.y / delta_pos.getLength()
    };
    Vec2Dim<F32> direct_a = {
        direct_b.y,
        -direct_b.x,
    };
    
    F32 cos_direct_a = ball_a->vel.innerProduct(direct_a)
        / (result_vel.getLength() * direct_a.getLength());
    F32 cos_direct_b = ball_a->vel.innerProduct(direct_b)
        / (result_vel.getLength() * direct_b.getLength());
    // NOTE(annad): Apply
    ball_a->vel = direct_a * result_vel.getLength() * cos_direct_a;
    ball_b->vel = direct_b * result_vel.getLength() * cos_direct_b;
    
    Assert(ball_b->vel.x > -1000);
    
    // DEBUG_DIRECTION_BALL_A = direct_a;
    // DEBUG_DIRECTION_BALL_B = direct_b;
    // DEBUG_DIRECTION_BALL_A_ID = ball_a->id;
    // DEBUG_DIRECTION_BALL_B_ID = ball_b->id;
}

void pq_init(PriorityQueue *pq)
{
    pq->size = PQ_MAX_ITEMS_COUNT;
    pq->cursor = 0;
}

void pq_clear(PriorityQueue *pq)
{
    pq->cursor = 0;
}

void pq_push(PriorityQueue *pq, CollideInfo *item)
{
    Assert(pq->size >= pq->cursor + 1);
    pq->items[pq->cursor] = *item;
    pq->cursor += 1;
}

S32 pq_peek(PriorityQueue *pq)
{
    F32 minDt = 1.0f; // TODO(annad): Incapsulate it????
    S32 idx = -1;
    
    CollideInfo *ci;
    for(S32 i = 0; i < pq->cursor; i += 1)
    {
        ci = &pq->items[i];
        if(ci->dt < minDt && ci->dt > 0.0f)
        {
            idx = i;
            minDt = ci->dt;
        }
    }
    
    return idx;
}

CollideInfo pq_pop(PriorityQueue *pq)
{
    S32 idx = pq_peek(pq);
    CollideInfo item = pq->items[idx];
    for (S32 i = idx; i < pq->cursor; i += 1)
    {
        pq->items[i] = pq->items[i + 1];
    }
    pq->cursor -= 1;
    return item;
}

void pq_display(PriorityQueue *pq)
{
    if(pq->cursor >= 2)
    {
        printf("==============\n");
    }
    
    for (S32 i = 0; i < pq->cursor; i += 1)
    {
        EvalPrintF(pq->items[i].dt);
    }
    
    if(pq->cursor >= 2)
    {
        printf("==============\n");
    }
}

// update for ball A with all balls
void update_collisions_pq(PriorityQueue *pq, Ball *balls, Ball *updated_ball)
{
    pq_clear(pq);
    Ball *ball_a = &balls[updated_ball->id];
    for (S32 i = 0; i < BALL_COUNT; i += 1)
    {
        if(updated_ball->id == i) continue;
        Ball *ball_b = &balls[i];
        if(balls_is_collide(updated_ball, ball_b))
        {
            CollideInfo ci;
            ci.ball_a_idx = updated_ball->id;
            ci.ball_b_idx = i;
            ci.dt = t_before_collide(ball_a, ball_b);
            pq_push(pq, &ci);
        }
    }
}
*/

#define BALL_SPEED 2500.0f
#define BALL_FRICTION 3.0f
// #define BALL_RADIUS 20.0f
// TODO(annad): ODE?
#define __STUB_CALC_ACCELERATION(Vel) (Vel * (-BALL_FRICTION))
internal void _ballsInit(Entity *balls, F32 x, F32 y)
{
    balls[CUE_BALL].id = CUE_BALL;
    balls[CUE_BALL].p.x = x;
    balls[CUE_BALL].p.y = y;
    balls[CUE_BALL].v.x = 0.0f;
    balls[CUE_BALL].v.y = 0.0f;
    balls[CUE_BALL].isInit = true;
    balls[CUE_BALL].isUpdated = false;
    balls[CUE_BALL].dtUpdate = 0.0f;
    balls[BALL_15].id = BALL_15;
    balls[BALL_15].p.x = balls[CUE_BALL].p.x + 3.0f * BALL_RADIUS;
    balls[BALL_15].p.y = balls[CUE_BALL].p.y - BALL_RADIUS;
    balls[BALL_15].v.x = 0.0f;
    balls[BALL_15].v.y = 0.0f;
    balls[BALL_15].isInit = true;
    balls[BALL_15].isUpdated = false;
    balls[BALL_15].dtUpdate = 0.0f;
    balls[BALL_14].id = BALL_14;
    balls[BALL_14].p.x = balls[CUE_BALL].p.x + 3.0f * BALL_RADIUS;
    balls[BALL_14].p.y = balls[CUE_BALL].p.y + BALL_RADIUS;
    balls[BALL_14].v.x = 0.0f;
    balls[BALL_14].v.y = 0.0f;
    balls[BALL_14].isInit = true;
    balls[BALL_14].isUpdated = false;
    balls[BALL_14].dtUpdate = 0.0f;
}

#if 1
internal void ballsInit(Entity *balls, F32 x, F32 y)
{
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
            balls[ballIdx].p.x = (x + (F32)i * (2.0f * (BALL_RADIUS)));
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
#endif

internal Entity ballUpdate(Entity *ball, F32 dt)
{
    V2DF32 a = __STUB_CALC_ACCELERATION(ball->v);
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

internal void ballHandleTableBoard(Entity *ball, Rect *table, F32 dt)
{
#if BILL_CFG_DEV_MODE
    localv S32 dbg_Count = 0;
    dbg_Count = 0;
#endif 
    Entity updated = ballUpdate(ball, dt);
    V2DF32 nvecwall = {};
    while (ballCheckTableBoardCollide(&updated, table, &nvecwall))
    {
#if BILL_CFG_DEV_MODE
        DbgPrint("[COLLIDE] >Solve, wall (%d)", ++dbg_Count);
#endif
        updated = *ball;
        updated.v -= nvecwall * 2.0f * updated.v.inner(nvecwall);
        ball->v = updated.v;
        updated = ballUpdate(ball, dt);
    }
}

internal void eventQueueClear(CollideEventQueue *cequeue)
{
    cequeue->pointer = 0;
}

/*
internal S32 evenetQueuePeek(CollideEventQueue *cequeue)
{
    F32 minTime = f32Infinity();
    S32 idx = -1;
    CollideEvent *event;
    for (S32 i = 0; i < cequeue->cursor; i += 1)
    {
        event = &cequeue->items[i];
        if (event->timeBefore < minTime && event->timeBefore >= 0.0f)
        {
            idx = i;
            minTime = colinfo->timeBefore;
        }
    }

    return idx;
}

internal BallsCollide eventQueuePop(CollideEventQueue *cequeue, S32 idx)
{
    Assert(cequeue->cursor > idx);
    CollideEvent swap = cequeue->items[idx];
    cequeue->items[idx] = cequeue->items[cequeue->cursor];
    cequeue->cursor -= 1;
    return swap;
}
*/

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
    if (dl < (2.0f * (F32)BALL_RADIUS))
    {
        // NOTE(annad): Already collide!
        return 0.0f;
    }

    F32 v = ballA->v.getLength(); // NOTE(annad): 
    if (f32EpsCompare(v, 0.0f, 0.01f)) // TODO(annad): kowalski analysis.
    {                                  // is this at all possible?
        // NOTE(annad): Never collide!
        return f32Infinity();
    }

    F32 cos = ballA->v.inner(d) / (dl * v);
    if (f32EpsCompare(cos, 0.0f, 0.001f))
    {
        // TODO(annad): Check this later
        // if one of the values goes to zero we get this case
        Assert(false);
        // return 0.0f;
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

    F32 a = 0.5f * __STUB_CALC_ACCELERATION(ballA->v).getLength();
    F32 D = f32Sqrt(f32Square(v) - 4.0f * a * s);
    Assert(D == D); // TODO(annad): If to f32Sqrt pass x <= 0
    F32 t = (v - D) / (2.0f * a);
    
    return t;
}

/*
internal void 
ballsScanCollidesBalls(Entity *balls, PQCollides *pqcollides)
{
#if BILL_CFG_DEV_MODE
    localv S32 dbg_Count = 0;
    localv S32 dbg_LocalFrameCounter = 0;
    localv B8 dbg_CollidesDetected = false;
#endif
    pqCollidesReset(pqcollides);
    for (S32 i = 0; i < BALL_COUNT; i += 1)
    {
        Entity *a = &balls[i];
        Entity updated = ballUpdate(a, a->dtUpdate);
        for (S32 j = 0; j < BALL_COUNT; j += 1)
        {
            if (i == j) continue;
            Entity *b = &balls[j];
            if (b->isInit && !b->isUpdated && ballCheckBallCollide(&updated, b))
            {
#if BILL_CFG_DEV_MODE
                dbg_CollidesDetected = true;
#endif
                BallsCollide colinfo;
                colinfo.idxBallA = a->id;
                colinfo.idxBallB = b->id;
                colinfo.timeBefore = ballTimeBeforeBallCollide(a, b);
                pqCollidesPush(pqcollides, &colinfo);
            }
        }
    }
#if BILL_CFG_DEV_MODE
    if (!dbg_CollidesDetected)
    {
        return;
    }

    dbg_CollidesDetected = false;
    if (dbg_LocalFrameCounter != dbg_GlobalFrameCounter)
    {
        dbg_LocalFrameCounter = dbg_GlobalFrameCounter;
        dbg_Count = 1;
    }
    else
    {
        dbg_Count += 1;
    }
    
    if (pqcollides->cursor)
    {
        DbgPrint("[COLLIDE] Detect, wall (%d)\n", dbg_Count);
        BallsCollide *colinfo;
        for (S32 i = 0; i < pqcollides->cursor; i += 1)
        {
            colinfo = &pqcollides->items[i];
            DbgPrint("colinfo[%d] a = %d, b = %d, dt = %f\n", 
                     i, colinfo->idxBallA, colinfo->idxBallB, 
                     colinfo->timeBefore);
        }
    }
#endif
}
*/

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
#if 0
            updated = ballUpdate(b, b->dtUpdate);
            if (ballCheckTableBoardCollide(&updated, table, e.nvecwall))
            {
#if BILL_CFG_DEV_MODE
                DbgPrint("[COLLIDE] >Detected, wall (eid %d)", b->id);
#endif
                e.eid = b->id;
                e.type = COLLIDE_WALL;
                e.dtBefore = ballTimeBeforeWallCollide(b, table, &e.nvecwall);
                eventQueuePush(&cequeue, &e);
            }
        }
    }
    
    S32 eventidx = evenetQueuePeek(&cequeue);
    if (eventidx == -1)
    {
        return false;
    }
    
    *colevent = eventQueuePop(&cequeue, eventidx);
    eventQueueClear(&cequeue);

    m_arena_pop_to(arena, arenaStartPosition);
    return true;

/*
    Entity updated = {};
    Rect *table = &gstate->table;
    Entity *balls = (Entity*)&gstate->balls;

    Entity *balls = &gstate->balls;
    // NOTE(annad): Error, out of memory!
    Assert(sizeof(PQCollides) <= COLLIDE_EVENT_CTX_SIZE);
    PQCollides pqcol = {};
    pqcol->items = 
    PQCollides *pqcol = (U8*)m_arena_push(arena, PQ_COLLIDES_SIZE);
    ballsScanCollides(balls, pqcol);
    S32 peak = pqCollidesPeek(pqcol);
    
    if (peak != -1)
    {
        BallsCollide masterColinfo = pqCollidesPop(pqcol, peak);
        BallsCollide *slaveColinfo = NULL;
        for (S32 i = 0; i < pqcol->cursor; i += 1)
        {
            BallsCollide *colinfo = &pqcol->items[i];
            if (masterColinfo.idxBallA == colinfo->idxBallA && 
                f32EpsCompare(masterColinfo.timeBefore, colinfo->timeBefore, 0.01f))
            {
                slaveColinfo = colinfo;
            }
        }
        
        Entity *ballA = &balls[masterColinfo.idxBallA];
        Entity *ballB = &balls[masterColinfo.idxBallB];
        // TODO(annad): https://physics.stackexchange.com/questions/296767/multiple-colliding-balls
        if (slaveColinfo)
        {
            Entity *ballC = &balls[slaveColinfo->idxBallB];
            colevent->type = COLLIDE_ONE_BALLS;
            return;
        }

        colevent->type = COLLIDE_TWO_BALLS;
        return;
    }
*/

#endif
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
        // Balls
        //
        F32 rackPosX = (0.75f * (F32)hRenderer->wScreen) - 5.0f * BALL_RADIUS;
        F32 rackPosY = (0.5f * (F32)hRenderer->hScreen) - 5.0f * BALL_RADIUS;
        // rackPosX = 100.0f;
        // rackPosY = 100.0f;
        ballsInit(balls, rackPosX, rackPosY);
        
        //
        // Table
        //
        Rect table = {
            0, 0, 
            hRenderer->wScreen, 
            hRenderer->hScreen
        };

        gstate->table = table;

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
        F32 rackPosX = (0.75f * (F32)hRenderer->wScreen) - 5.0f * BALL_RADIUS;
        F32 rackPosY = (0.5f * (F32)hRenderer->hScreen) - 5.0f * BALL_RADIUS;
        // rackPosX = 100.0f;
        // rackPosY = 100.0f;

        ballsInit(balls, rackPosX, rackPosY);
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
                if (f32EpsCompare(colevent.dtBefore, 0.0f, 0.001f))
                {
                    // TODO(annad): It's not solve problem, just hide here
                    a->isUpdated = true;
                }
                *a = ballUpdate(a, colevent.dtBefore);
                a->dtUpdate -= colevent.dtBefore;
                ballSolveCollideOneBall(a, b);
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
        if (e->isInit)
        {
            *e = ballUpdate(e, e->dtUpdate);
            e->dtUpdate = 0.0f;
            e->isUpdated = true;
        }
    }

    Renderer_pushCmd(hRenderer, RCMD_SET_RENDER_COLOR, 0xff, 0xff, 0xff, 0xff);
    for (S32 i = 0; i < BALL_COUNT; i += 1)
    {
        Entity *e = &balls[i];
        if (e->isInit)
        {
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

    Renderer_pushCmd(hRenderer, RCMD_NULL);
}

/*
void game_update_and_render(GameMemory *game_memory, 
                            Renderer *renderer,
                            GameInput *game_input, 
                            GameTime *game_time)
{
    (void)game_input;
    (void)game_time;
    S32 const_ball_radius = 20;
    GameState *game_state = (GameState*)game_memory->permanent_storage;
    DEBUG_game_state = game_state;
    DEBUG_renderer = renderer;
    if(game_state->initialize_flag == false)
    {
        M_BaseMemory *m_vtbl = m_void_base_memory(game_memory->persistent_storage, 
                                                  game_memory->persistent_storage_size);
        game_state->arena = m_make_arena_reserve(m_vtbl, game_memory->persistent_storage_size);
        
        F32 pos_x = (F32)(renderer->context.width / 2) 
            + const_ball_radius * 3.0f 
            + (const_ball_radius * 1.5f) 
            - (const_ball_radius * 1.0f) 
            + (const_ball_radius * 0.5f) 
            - (const_ball_radius * 0.25f);
        F32 pos_y = (F32)(renderer->context.height / 2) + const_ball_radius * 2.0f;
        Ball *ball = &(game_state->balls[0]);
        ball->id = 0;
        ball->pos = { pos_x, pos_y };
        ball->vel = {};
        
        for(S32 i = 1, j = 0; i < BALL_COUNT; i++, j = (i + 1) / 3)
        {
            pos_x = (F32)(renderer->context.width / 2);
            pos_y = (F32)(renderer->context.height / 2);
            ball = &(game_state->balls[i]);
            ball->id = i;
            pos_x += (i * const_ball_radius * 2.0f + i * 10.0f);
            ball->pos = { pos_x, pos_y };
            ball->vel = {};
        }        
        
        game_state->bill_cue = {};
        game_state->initialize_flag = true;
        
        pq_init(&game_state->pq);
    }
    
    // MemArena *memory_arena = &game_state->memory_arena;
    if(game_input->keyboard.keys[INPUT_KEYBOARD_KEYS_RETURN])
    {
        F32 pos_x = (F32)(renderer->context.width / 2) 
            + const_ball_radius * 3.0f 
            + (const_ball_radius * 1.5f) 
            - (const_ball_radius * 1.0f) 
            + (const_ball_radius * 0.5f) 
            - (const_ball_radius * 0.25f);
        F32 pos_y = (F32)(renderer->context.height / 2) + const_ball_radius * 2.0f;
        Ball *ball = &(game_state->balls[0]);
        ball->id = 0;
        ball->pos = { pos_x, pos_y };
        ball->vel = {};
        
        for(S32 i = 1, j = 0; i < BALL_COUNT; i++, j = (i + 1) / 3)
        {
            pos_x = (F32)(renderer->context.width / 2);
            pos_y = (F32)(renderer->context.height / 2);
            ball = &(game_state->balls[i]);
            ball->id = i;
            pos_x += (i * const_ball_radius * 2.0f + i * 10.0f);
            ball->pos = { pos_x, pos_y };
            ball->vel = {};
        }
    }
    
    GameInputMouse *mouse = &game_input->mouse;
    localv S32 FRAME_COUNTER_AFTER_CUE = 0;
    if(mouse->buttons_states[INPUT_MOUSE_BUTTON_LEFT].state == INPUT_BUTTON_STATE_DOWN)
    {
        F32 bill_cue_x = ((F32)(mouse->buttons_states[INPUT_MOUSE_BUTTON_LEFT].click_pos.x - mouse->cursor_pos.x));
        F32 bill_cue_y = ((F32)(mouse->buttons_states[INPUT_MOUSE_BUTTON_LEFT].click_pos.y - mouse->cursor_pos.y));
        
        game_state->bill_cue = {
            bill_cue_x,
            bill_cue_y
        };
        
        // PRINT_VEC(game_state->bill_cue);
    }
    else
    {
        if(game_state->bill_cue.getLength())
        {
            game_state->balls[BALL_WHITE].vel = game_state->bill_cue;
            
            FRAME_COUNTER_AFTER_CUE = 0;
            game_state->bill_cue = {};
        }
    }
    
    // TODO(annad): Move handling...
    Ball *balls = (Ball*)game_state->balls;
    PriorityQueue *pq = &game_state->pq;
    for(S32 i = 0; i < BALL_COUNT; i += 1)
    {
        // WALL COLLISIONS
        Ball *ball_a = &balls[i];
        Ball updated_ball = update_ball(ball_a, 1.0f/30.0f);
        B8 rightWallFlag = updated_ball.pos.x / renderer->context.width >= 1.0f;
        B8 leftWallFlag = updated_ball.pos.x / renderer->context.width <= 0.0f;
        B8 upWallFlag = updated_ball.pos.y / renderer->context.height >= 1.0f;
        B8 downWallFlag = updated_ball.pos.y / renderer->context.height <= 0.0f;
        while (rightWallFlag || leftWallFlag || upWallFlag || downWallFlag)
        {
            Vec2Dim<F32> nVecWall = {};
            if (rightWallFlag)
            {
                nVecWall = {1.0f, 0.0f};
            }

            if (leftWallFlag)
            {
                nVecWall = {-1.0f, 0.0f};
            }

            if (upWallFlag)
            {
                nVecWall = {0.0f, -1.0f};
            }

            if (downWallFlag)
            {
                nVecWall = {0.0f, 1.0f};
            }

            PRINT_VEC(nVecWall);
            updated_ball = *ball_a;
            updated_ball.vel -= nVecWall * 2.0f * updated_ball.vel.innerProduct(nVecWall);
            ball_a->vel = updated_ball.vel;

            updated_ball = update_ball(ball_a, 1.0f/30.0f);
            rightWallFlag = updated_ball.pos.x / renderer->context.width >= 1.0f;
            leftWallFlag = updated_ball.pos.x / renderer->context.width <= 0.0f;
            upWallFlag = updated_ball.pos.y / renderer->context.height >= 1.0f;
            downWallFlag = updated_ball.pos.y / renderer->context.height <= 0.0f;
        }
        // WALL COLLISIONS

        update_collisions_pq(pq, balls, &updated_ball);
        S32 itemid = pq_peek(pq);
        if(itemid != -1)
        {
            pq_display(pq);
            CollideInfo ci = pq_pop(pq);
            updated_ball = update_ball(ball_a, ci.dt);
            Ball *ball_b = &balls[ci.ball_b_idx];
            S32 itemid2 = pq_peek(pq);
            if (itemid2 != -1)
            {
                CollideInfo ci2 = pq_pop(pq);
                F32 eps = 0.01f;
                if (f32Abs(ci2.dt - ci.dt) < eps)
                {
                    Ball *ball_c = &balls[ci2.ball_b_idx];
                    three_balls_collide_handle(&updated_ball, ball_b, ball_c);

                }
            }
            else
            {
                two_balls_collide_handle(&updated_ball, ball_b);
            }
        }
        
        *ball_a = updated_ball;
        // printf("---------------\n");
        // EvalPrint(ball_a->id);
        // EvalPrint(ball_a->vel.x);
        // EvalPrint(ball_a->vel.y);
    }
    // Assert(balls[0].vel.x > -1000);
    //
    //
    // 
    
    DEBUG_RENDER_DEBUG_INFORMATION();
    
    for(S32 i = 0; i < BALL_COUNT; i++)
    {
        Ball *iter_ball = &game_state->balls[i];
        if(i == BALL_WHITE)
        {
            RGBA_U8 c = {0xff, 0xff, 0xff, 0xff};
            renderer_push_command(renderer, RENDERER_COMMAND_SET_RENDER_COLOR, &c);        
        }
        else
        {
            RGBA_U8 c = {0x00, 0xff, 0x0, 0xff};
            renderer_push_command(renderer, RENDERER_COMMAND_SET_RENDER_COLOR, &c);
        }
        
        renderer_push_command(renderer, RENDERER_COMMAND_DRAW_CIRCLE, 
                              (S32)iter_ball->pos.x, 
                              (S32)iter_ball->pos.y, 
                              const_ball_radius);
    }
    
    RGBA_U8 cue_color = {0xff, 0x00, 0xff, 0xff};
    renderer_push_command(renderer, RENDERER_COMMAND_SET_RENDER_COLOR, &cue_color);
    renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE,
                          (S32)game_state->balls[0].pos.x,
                          (S32)game_state->balls[0].pos.y,
                          (S32)game_state->balls[0].pos.x + ((S32)game_state->bill_cue.x),
                          (S32)game_state->balls[0].pos.y + ((S32)game_state->bill_cue.y));
}
*/

