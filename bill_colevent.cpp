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
    CollideEventQueue *queue = &gstate->cequeue;
    F32 radius = gstate->balldiam / 2.0f;
    eventQueueClear(queue);
    Entity updated = {};
    Rect *tableCollider = &gstate->table.collider;
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
            if (ballCheckBallCollide(&updated, b, radius))
            {
                e.eid = a->id;
                e.type = COLLIDE_BALL_BALL;
                e.dtBefore = ballTimeBeforeBallCollide(a, b, radius);
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

