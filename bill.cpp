/* 
Author: github.com/annadostoevskaya

File: bill.cpp
Date: September 24th 2022 8:05 pm 
Description: <empty>
*/

#include "core/math.h"
#include "core/memory.h"
#include "core/memory_void.cpp"
#include "core/memory.cpp"
#include "bill_renderer.cpp"
#include "bill_math.h"
#include "bill.h"

#define BALL_SPEED 2500.0f
#define BALL_FRICTION 3.0f
// #define BALL_RADIUS 20.0f

#include "_debug.cpp"
#pragma warning(disable : 4701)
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

internal void ballsInit(Entity *balls, S32 x, S32 y)
{
    S32 dy = 5;
    S32 dx = 5;
    S32 ballIdx = BALL_2;
    for (S32 i = dx; i > 0; i -= 1)
    {
        dy = i;
        S32 shift = ((5 - i) * BALL_RADIUS);
        for (S32 j = dy; j > 0; j -= 1)
        {
            Assert(ballIdx < BALL_COUNT);
            balls[ballIdx].p.y = (F32)(y + shift + j * (2 * BALL_RADIUS));
            balls[ballIdx].p.x = (F32)(x + i * (2 * BALL_RADIUS));
            balls[ballIdx].isInit = true;
            ballIdx += 1;
        }
    }

    balls[CUE_BALL].p.x = balls[BALL_16].p.x - (F32)(2 * BALL_RADIUS * 10);
    balls[CUE_BALL].p.y = balls[BALL_16].p.y;
    balls[CUE_BALL].isInit = true;
}

internal void gtick(GameIO *io)
{
    // NOTE(annad): Platform layer
    GameStorage *storage = io->storage;
    GameState *gstate = (GameState*)storage->permanent;
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
        S32 rackPosX = (S32)(0.75f * (F32)hRenderer->wScreen) - 5 * BALL_RADIUS;
        S32 rackPosY = (S32)(0.5f * (F32)hRenderer->hScreen) - 5 * BALL_RADIUS;
        ballsInit(balls, rackPosX, rackPosY);

        gstate->isInit = true;
    }
    
    if (devices->keybBtns[KEYB_BTN_RETURN])
    {
        // ...
    }

    if (devices->mouseBtns[MOUSE_BTN_LEFT])
    {
        if (!cuestick->isInit)
        {
            cuestick->pin.x = devices->mouseX;
            cuestick->pin.y = devices->mouseY;
            cuestick->isInit = true;
        }
    }
    
    if (!devices->mouseBtns[MOUSE_BTN_LEFT])
    {
        if (cuestick->isInit)
        {
            Entity *cueball = &balls[CUE_BALL];
            V2DF32 impact;
            impact.x = (F32)(cuestick->pin.x - devices->mouseX);
            impact.y = (F32)(cuestick->pin.y - devices->mouseY);
            cueball->v = impact;
        }
    }

    Renderer_pushCmd(hRenderer, RCMD_SET_RENDER_COLOR, 0xff, 0xff, 0xff, 0xff);
    for (S32 i = 0; i < BALL_COUNT; i += 1)
    {
        Entity *e = &balls[i];
        if (e->isInit)
        {
            Renderer_pushCmd(hRenderer, RCMD_DRAW_CIRCLE, e->p.x, e->p.y, BALL_RADIUS);
        }
    }

    Renderer_pushCmd(hRenderer, RCMD_SET_RENDER_COLOR, 0xff, 0x00, 0x00, 0xff);
    Renderer_pushCmd(hRenderer, RCMD_DRAW_LINE, 
            (F32)balls[CUE_BALL].p.x, 
            (F32)balls[CUE_BALL].p.y);
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

