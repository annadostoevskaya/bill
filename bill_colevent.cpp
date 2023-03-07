/*
Author: github.com/annadostoevskaya
File: bill_colevent.cpp
Date: 16/02/23 13:30:39

Description: <empty>
*/

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
#if 0 
    for (S32 i = 0; i < BALL_COUNT; i += 1)
    {
        Entity *b = &balls[i];
        if (b->isInit && !b->isUpdated)
        {
            updated = ballUpdate(b, b->dtUpdate);
            // NOTE(annad): Out of memory!
            StaticAssert(sizeof(V2DF32) <= sizeof(e.custom));
            V2DF32 *nvecwall = (V2DF32*)(e.custom);
            if (ballCheckTableBoardCollide(&updated, radius, tableCollider, nvecwall))
            {
#if BILL_CFG_DEV_MODE
                DbgPrint("[COLLIDE] >Detected, ball-wall (eid %d, dt %f)", b->id, e.dtBefore);
#endif
                e.eid = b->id;
                e.type = COLLIDE_BALL_WALL;
                e.dtBefore = ballTimeBeforeWallCollide(b, tableCollider, nvecwall);
                eventQueuePush(queue, &e);
            }
        }
    }
#endif
    CollideEventQueue *queue = &gstate->cequeue;
    eventQueueClear(queue);
    CollideEvent e = {};

    F32 radius = gstate->radius;
    Entity *balls = (Entity*)(&gstate->balls);
    Table *table = &gstate->table;

    Entity updated = {};
    for (S32 i = 0; i < BALL_COUNT; i += 1)
    {
        Entity *ball = &balls[i];
        if (!ball->isInit || ball->isUpdated) continue;
        updated = ballUpdate(ball, ball->dtUpdate);
#if 1
        for (S32 j = 0; j < sizeof(table->boards) / sizeof(table->boards[0]); j += 1)
        {
            for (S32 k = 0; k < sizeof(table->boards[0]) / sizeof(table->boards[0].p[0]) - 1; k += 1)
            {
                P2DF32 a = table->boards[j].p[k];
                P2DF32 b = table->boards[j].p[k+1];
                V2DF32 line = b - a;
                V2DF32 nx = line.getNormalize(); 
                V2DF32 ny = {-nx.y, nx.x};
                V2DF32 p = {
                    (updated.p - a).inner(nx), 
                    (updated.p - a).inner(ny),
                };
                
                if (radius >= p.y && p.x >= -radius && 
                    p.x <= line.getLength() + radius)
                {
                    printf("{%f,%f}\n", updated.p.x, updated.p.y);
                    e.eid = ball->id;
                    e.type = COLLIDE_BALL_WALL;
                    e.dtBefore = 0.0f;
                    StaticAssert(sizeof(V2DF32) <= sizeof(e.custom));
                    V2DF32 *nvecwall = (V2DF32*)(e.custom);
                    *nvecwall = ny;
                    eventQueuePush(queue, &e);
#if BILL_CFG_DEV_MODE
                    DbgPrint("[COLLIDE] >Detected, ball-wall (eid %d, dt %f)", ball->id, e.dtBefore);
#endif
                }
            }
        }
#endif
        for (S32 j = 0; j < BALL_COUNT; j += 1)
        {
            if (i == j) continue;
            Entity *b = &balls[j];
            if (!b->isInit) continue;
            if (ballCheckBallCollide(&updated, b, radius))
            {
                e.eid = ball->id;
                e.type = COLLIDE_BALL_BALL;
                e.dtBefore = ballTimeBeforeBallCollide(ball, b, radius);
                S32 *ballbeid = (S32*)e.custom;
                *ballbeid = b->id;
                eventQueuePush(queue, &e);
#if BILL_CFG_DEV_MODE
                DbgPrint("[COLLIDE] >Detected, ball-ball (a_eid %d, b_eid %d, dt %f)", ball->id, b->id, e.dtBefore);
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

