/* 
Author: github.com/annadostoevskaya

File: bill.cpp
Date: September 24th 2022 8:05 pm 
Description: <empty>
*/

#include "bill_renderer.cpp"
#include "bill_math.h"
#include "bill.h"

#define BALL_SPEED 2500.0f
#define BALL_FRICTION 3.0f
#define BALL_RADIUS 20.0f
Ball update_ball(Ball *ball, Vec2Dim<F32> *acc, F32 dt)
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
    Ball updated_ball = *ball;
    updated_ball.pos = ball->pos + (*acc) * 0.5f * square(dt) + ball->vel * dt;
    updated_ball.vel = ball->vel + (*acc) * dt;
    return updated_ball;
}

// TODO(annad): Refactoring...
B32 balls_is_collide(Ball *ball_a, Ball *ball_b)
{
    F32 distance = (ball_a->pos - ball_b->pos).getLength();
    return distance < (2.0f * BALL_RADIUS);
}


F32 dt_before_collide(Ball *ball_a, Ball *ball_b, Vec2Dim<F32> *ball_a_acc)
{
    F32 t = 0.0f;
    
    Vec2Dim<F32> delta_pos = ball_b->pos - ball_a->pos;
    F32 cos_fi = ball_a->vel.innerProduct(delta_pos) 
        / (delta_pos.getLength() * ball_a->vel.getLength());
    if(cos_fi != cos_fi) // TODO(annad): is_infinity()?
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
        const F32 epsilon = 0.001f;
        if(f32Abs(cos_fi  - 1.0f) < epsilon)
        {
            s = delta_pos.getLength() - (2.0f * BALL_RADIUS);
        }
        else
        {
            // F32 cos_fi = ball_a->vel.innerProduct(delta_pos) 
            // / (delta_pos.getLength() * ball_a->vel.getLength());
            F32 fi_angle = defaultArcCos(cos_fi); // angle delta_pos,v
            F32 side_a = 2.0f * BALL_RADIUS; // 2r 
            F32 side_b = delta_pos.getLength(); // |B - A|
            F32 sin_a = defaultSin(fi_angle);
            F32 sin_b = (sin_a / side_a) * side_b;
            F32 sin_c = defaultSin(defaultArcSin(sin_b) - fi_angle);
            F32 side_c = (side_a / sin_a) * sin_c; // s before collide
            s = side_c;
            Assert(s == s); // TODO(annad): Collision order!
            Assert(s > 0.0f);
        }
        
        F32 v = ball_a->vel.getLength();
        F32 a = 0.5f * ball_a_acc->getLength();
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
        MemArena *memory_arena = &game_state->memory_arena;
        memory_arena->base = game_memory->persistent_storage;
        memory_arena->size = game_memory->persistent_storage_size;
        memory_arena->pos = 0;
        
        for(S32 i = 0; i < BALL_ENUM_COUNT; i++)
        {
            F32 pos_x = (F32)(2 * const_ball_radius * i);
            F32 pos_y = (F32)const_ball_radius;
            Ball *ball = &(game_state->balls[i]);
            ball->id = i;
            ball->pos = {
                pos_x + renderer->context.width / 2,
                pos_y + renderer->context.height / 2,
            };
            ball->vel = {};
        }
        
        game_state->initialize_flag = true;
        game_state->DEBUG_pause_game = false;
    }
    
    // MemArena *memory_arena = &game_state->memory_arena;
    
    if(game_input->keyboard.keys[INPUT_KEYBOARD_KEYS_RETURN])
    {
        for(S32 i = 0; i < BALL_ENUM_COUNT; i++)
        {
            F32 pos_x = (F32)(2 * const_ball_radius * i);
            F32 pos_y = (F32)const_ball_radius;
            game_state->balls[i].pos = {
                pos_x + renderer->context.width / 2,
                pos_y + renderer->context.height / 2,
            };
            game_state->balls[i].vel = {};
        }
        
        game_state->DEBUG_pause_game = false;
    }
    
    Vec2Dim<F32> ball_control_acc = {};
    F32 dts_before_collide[BALL_ENUM_COUNT][BALL_ENUM_COUNT] = {};
    if(!game_state->DEBUG_pause_game)
    {
        if(game_input->keyboard.keys[INPUT_KEYBOARD_KEYS_S])
        {
            ball_control_acc.y = 1.0f;
        }
        
        if(game_input->keyboard.keys[INPUT_KEYBOARD_KEYS_W])
        {
            ball_control_acc.y = -1.0f;
        }
        
        if(game_input->keyboard.keys[INPUT_KEYBOARD_KEYS_A])
        {
            ball_control_acc.x = -1.0f;
        }
        
        if(game_input->keyboard.keys[INPUT_KEYBOARD_KEYS_D])
        {
            ball_control_acc.x = 1.0f;
        }
        
        if((ball_control_acc.x != 0.0f) && (ball_control_acc.y != 0.0f))
        {
            ball_control_acc *= 0.70710678118f;
        }
        
        F32 dt = (((F32)game_time->dt / 1000.0f));
        for(S32 i = 0; i < BALL_ENUM_COUNT; i += 1)
        {
            Ball *ball_a = &game_state->balls[i];
            Vec2Dim<F32> ball_a_acc = {};
            if(i == BALL_ENUM_WHITE) 
            {
                ball_a_acc = ball_control_acc * BALL_SPEED; // TODO(annad): Remove this.
            }
            
            ball_a_acc += ball_a->vel * (-BALL_FRICTION);
            Ball updated_ball_a = update_ball(ball_a, &ball_a_acc, dt);
            for(S32 j = i + 1; j < BALL_ENUM_COUNT; j += 1)
            {
                Ball *ball_b = &(game_state->balls[j]);
                
                if(balls_is_collide(&updated_ball_a, ball_b))
                {
                    F32 t = dt_before_collide(ball_a, ball_b, &ball_a_acc);
                    dts_before_collide[i][j] = t;
                    // updated_ball_a = update_ball(ball_a, &ball_a_acc, t);
                    // balls_collide_handle(&updated_ball_a, ball_b);
                }
            }
            
            *ball_a = updated_ball_a;
        }
    }
    
    for(S32 i = 0; i < BALL_ENUM_COUNT; i += 1)
    {
        for(S32 j = i + 1; j < BALL_ENUM_COUNT; j += 1)
        {
            printf("%f ", dts_before_collide[i][j]);
        }
        printf("\n");
    }
    
    
    for(S32 i = 0; i < BALL_ENUM_COUNT; i += 1)
    {
        for(S32 j = i + 1; j < BALL_ENUM_COUNT; j += 1)
        {
            if(dts_before_collide[i][j] != 0.0f)
            {
                __debugbreak();
            }
        }
    }
    
    
    Ball *white_ball = &game_state->balls[BALL_ENUM_WHITE];
    
    RGBA_U8 color{0xff, 0x0, 0x0, 0xff};
    renderer_push_command(renderer, 
                          RENDERER_COMMAND_SET_RENDER_COLOR,
                          &color);
    renderer_push_command(renderer, RENDERER_COMMAND_DRAW_CIRCLE, 
                          (S32)f32Round(white_ball->pos.x), 
                          (S32)f32Round(white_ball->pos.y), 
                          const_ball_radius);
    
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
}

