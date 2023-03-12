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
handleTableBoard(Entity *updated, F32 radius, 
    TableBoard *board, U32 numPoints, 
    CollideEvent *e)
{
    CollideEvent colevent = {};
    V2DF32 nvecwall = {};
    for (U32 k = 0; k < numPoints - 2; k += 1)
    {
        P2DF32 a = board->p[k];
        P2DF32 b = board->p[k+1];
        P2DF32 c = board->p[k+2];
        if (ballCheckLineCollide(updated, radius, a, b, &nvecwall))
        {
            colevent.eid = updated->id;
            StaticAssert(sizeof(colevent.custom.v2df32) == sizeof(V2DF32));
            colevent.custom.v2df32 = nvecwall;
            colevent.type = COLLIDE_BALL_WALL;
            colevent.dtBefore = 0.0f;
            *e = colevent;
            return true;
        }

        if (ballCheckLineCollide(updated, radius, b, c, &nvecwall))
        {
            colevent.eid = updated->id;
            StaticAssert(sizeof(colevent.custom.v2df32) == sizeof(V2DF32));
            colevent.custom.v2df32 = nvecwall;
            colevent.type = COLLIDE_BALL_WALL;
            colevent.dtBefore = 0.0f;
            *e = colevent;
            return true;
        }

        if ((b - updated->p).getLength() <= radius)
        {
            nvecwall = (b - updated->p).getNormalize();
            colevent.eid = updated->id;
            StaticAssert(sizeof(colevent.custom.v2df32) == sizeof(V2DF32));
            colevent.custom.v2df32 = nvecwall;
            colevent.type = COLLIDE_BALL_WALL;
            colevent.dtBefore = 0.0f;
            *e = colevent;
            return true;
        }
    }

    return false;
}

internal B8
collideEventPoll(GameState *gstate, CollideEvent *colevent)
{
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
        for (S32 j = 0; j < sizeof(table->boards) / sizeof(table->boards[0]); j += 1)
        {
            TableBoard *board = &table->boards[j];
            U32 numPoints = sizeof(TableBoard) / sizeof(V2DF32);
            if (handleTableBoard(&updated, radius, board, numPoints, &e))
            {
                eventQueuePush(queue, &e);
#if BILL_CFG_DEV_MODE
                DbgPrint("[COLLIDE] >Detected, ball-board (eid %d, boardid %d, dt %f)", ball->id, j, e.dtBefore);
#endif
            }
        }

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
                e.custom.s32 = b->id;
                eventQueuePush(queue, &e);
#if !BILL_CFG_DEV_MODE
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

