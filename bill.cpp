/* 
Author: github.com/annadostoevskaya

File: bill.cpp
Date: September 24th 2022 8:05 pm 
Description: <empty>
*/


// ================== TRASH ==================
#define _GET_MIN(Arr, ArrSz, Min) \
for(S32 _i = 0; _i < ArrSz; _i += 1) \
{ \
\
if(Min > Arr[_i])  \
{ \
Min = Arr[_i]; \
} \
}

#define GET_MIN(Arr, Min) _GET_MIN(Arr, (sizeof(Arr) / sizeof(Arr[0])), Min)
// ================== TRASH ==================

#include "bill_renderer.cpp"
#include "bill_math.h"
#include "bill.h"

#define BALL_SPEED 2500.0f
#define BALL_FRICTION 3.0f
#define BALL_RADIUS 20.0f

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
    Vec2Dim<F32> ball_acc = ball->vel * (-BALL_FRICTION);
    Ball updated_ball = *ball;
    updated_ball.pos += ball_acc * 0.5f * square(dt) + ball->vel * dt;
    updated_ball.vel += ball_acc * dt;
    return updated_ball;
}

// TODO(annad): Refactoring...
B32 balls_is_collide(Ball *ball_a, Ball *ball_b)
{
    F32 distance = (ball_a->pos - ball_b->pos).getLength();
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
                EvalPrintF(sin_b);
                sin_b = 1.0f;
            }
            else if(sin_b < -1.0f)
            {
                EvalPrintF(sin_b);
                sin_b = -1.0f;
            }
            
            F32 sin_c = defaultSin(defaultArcSin(sin_b) - fi_angle); 
            F32 side_c = (side_a / sin_a) * sin_c; // s before collide
            s = side_c;
            // TODO(annad): Collision order!
            EvalPrintF(s);
            Assert(s == s); // NOTE(annad): S = [2.0f ~ 21.0f], when V -> inf.
            Assert(s > 0.0f);
        }
        
        F32 v = ball_a->vel.getLength();
        F32 a = 0.5f * ball_a_acc.getLength();
        EvalPrintF(a);
        F32 discriminant = (F32)sqrt(square(v) - 4.0f * a * s);
        Assert(discriminant == discriminant);
        // TODO(annad): math, sqrt
        t = (v - discriminant) / (2.0f * a); // NOTE(annad): what is t < 0.0f?
    }
    
    return t;
}

void balls_collide_handle(Ball *ball_a, Ball *ball_b)
{
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
    F32 result_vel = ball_a->vel.getLength();
    F32 cos_direct_a = ball_a->vel.innerProduct(direct_a)
        / (result_vel * direct_a.getLength());
    F32 cos_direct_b = ball_a->vel.innerProduct(direct_b)
        / (result_vel * direct_b.getLength());
    // NOTE(annad): Apply
    ball_a->vel = direct_a * result_vel * cos_direct_a;
    ball_b->vel += direct_b * result_vel * cos_direct_b;
}

void get_table_t_before_collide(F32 *t_table, Ball *balls, F32 dt)
{
    for(S32 i = 0; i < BALL_ENUM_COUNT; i += 1)
    {
        Ball *ball_a = &(balls[i]);
        Vec2Dim<F32> ball_a_acc = ball_a->vel * (-BALL_FRICTION);
        Ball updated_ball_a = update_ball(ball_a, dt);
        for(S32 j = 0; j < BALL_ENUM_COUNT; j += 1)
        {
            if(i == j)
            {
                continue;
            }
            
            Ball *ball_b = &(balls[j]);
            if(balls_is_collide(&updated_ball_a, ball_b))
            {
                t_table[i * BALL_ENUM_COUNT + j] = t_before_collide(ball_a, ball_b);
            }
        }
    }
}


void game_update_and_render(GameMemory *game_memory, 
                            Renderer *renderer, 
                            GameInput *game_input, 
                            GameTime *game_time)
{
    (void)game_input;
    (void)game_time;
    S32 const_ball_radius = 20;
    GameState *game_state = (GameState*)game_memory->permanent_storage;
    if(game_state->initialize_flag == false)
    {
        Arena *memory_arena = &game_state->memory_arena;
        memory_arena->base = game_memory->persistent_storage;
        memory_arena->size = game_memory->persistent_storage_size;
        memory_arena->pos = 0;
        
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
        
        for(S32 i = 1, j = 0; i < BALL_ENUM_COUNT; i++, j = (i + 1) / 3)
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
        
        for(S32 i = 1, j = 0; i < BALL_ENUM_COUNT; i++, j = (i + 1) / 3)
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
            game_state->balls[0].vel = game_state->bill_cue;
            game_state->bill_cue = {};
        }
    }
    
    // TODO(annad): Write Linked-List and Proroty-Queue
    F32 dt = (((F32)game_time->dt / 1000.0f));
    F32 dt_for_balls[BALL_ENUM_COUNT] = {};
    for(S32 i = 0; i < BALL_ENUM_COUNT; i += 1)
    {
        dt_for_balls[i] = dt;
    }
    
    B32 time_is_up_for_every_balls_flag = false;
    while(!time_is_up_for_every_balls_flag)
    {
        F32 t_before_collide[BALL_ENUM_COUNT * BALL_ENUM_COUNT] = {};
        get_table_t_before_collide(t_before_collide, (Ball*)&game_state->balls, dt);
        S32 idx_min = 0;
        F32 t_min = 999.0f;
        while(idx_min < BALL_ENUM_COUNT * BALL_ENUM_COUNT)
        {
            if(t_before_collide[idx_min] != 0.0f && t_min > t_before_collide[idx_min])
            {
                t_min = t_before_collide[idx_min];
            }
        }
        
        if(t_min == 999.0f)
        {
            // FEELING THAT I'M TRYING TO WRITE TWO DIFFERENT FUNCTIONS 
            // AT ONCE WHAT THE FUCK
            continue;
        }
        
        S32 a_idx = idx_min / BALL_ENUM_COUNT;
        S32 b_idx = idx_min % BALL_ENUM_COUNT;
        Ball *ball_a = &game_state->balls[a_idx];
        Ball *ball_b = &game_state->balls[b_idx];
        update_ball(ball_a, t_min);
        balls_collide_handle(ball_a, ball_b);
        dt_for_balls[a_idx] -= t_min;
        
        time_is_up_for_every_balls_flag = true;
        for(S32 idx = 0; idx < BALL_ENUM_COUNT; idx++)
        {
            if(dt_for_balls[idx] != 0.0f)
            {
                time_is_up_for_every_balls_flag = false;
            }
        }
    }
    
    for(S32 i = 0; i < BALL_ENUM_COUNT; i++)
    {
        Ball *iter_ball = &game_state->balls[i];
        if(i == BALL_ENUM_WHITE)
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


