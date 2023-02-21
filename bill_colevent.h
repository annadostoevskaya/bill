/*
Author: github.com/annadostoevskaya
File: bill_colevent.h
Date: 16/02/23 13:34:31

Description: <empty>
*/

enum CollideType
{
    // TODO(annad): https://physics.stackexchange.com/questions/296767/multiple-colliding-balls
    COLLIDE_NO,
    COLLIDE_BALL_WALL,
    COLLIDE_BALL_BALL,
    COLLIDE_TWO_BALL,

    COLLIDE_COUNT,
    COLLIDE_UNDEFINED
};

#define COLLIDE_EVENT_QUEUE_COUNT 32

struct CollideEvent
{
    EntityID eid;
    F32 dtBefore;
    CollideType type;
    
    U8 custom[8]; // NOTE(annad): Eight bytes for custom data.
};

struct CollideEventQueue
{
    CollideEvent *pool;
    S32 count;
    S32 pointer;
};