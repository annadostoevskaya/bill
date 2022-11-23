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
    Ball updated_ball;
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

void balls_collide_handle(Ball *ball_a, Ball *ball_b, Vec2Dim<F32> *ball_a_acc, F32 *dt)
{
    printf("======================[ COLLISION HANDLE START ]==========================\n");
    // TODO(annad): All acceleration params must 
    // work in function with explicit
    // setting or pass to func.
    // NOTE(annad): Recalculate positions in collision.
    Vec2Dim<F32> delta_pos = ball_b->pos - ball_a->pos;
    F32 cos_fi = ball_a->vel.innerProduct(delta_pos) / (delta_pos.getLength() * ball_a->vel.getLength());
    if(cos_fi != cos_fi) // TODO(annad): is_infinity()?
    {
        // TODO(annad): I don’t quite understand what can be done with this, 
        // but this case means that all the power of the ball has been absorbed.
        *dt = 0.0f;
    }
    else
    {
        // NOTE(annad): Get distance between collide points
        // s(fi) = 2r * csc(fi) * sin( arcsin(2r*S*csc(fi)) +  fi), 
        // fi - angle between Vec(B - A), Vec(Velocity)
        // r - ball radius
        // S - |B - A|
        F32 fi_angle = defaultArcCos(cos_fi);

        F32 side_a = 2.0f * BALL_RADIUS;
        F32 side_b = delta_pos.getLength();
        F32 sin_a = defaultSin(defaultArcCos(cos_fi));
        F32 sin_c = defaultSin(defaultArcSin((side_a * side_b) / defaultArcSin(sin_a)) + defaultArcSin(sin_c));

        F32 omega_angle = defaultArcSin(2.0f * BALL_RADIUS * delta_pos.getLength() * defaultCsc(fi_angle)) + fi_angle;
        F32 side_c = (side_a / sin_a) * sin_c;
        F32 s = side_c;
        __debugbreak();
        F32 v = ball_a->vel.getLength() * cos_fi;
        // F32 s = delta_pos.getLength() - 2.0f * BALL_RADIUS; 
        // TODO(annad): IT'S FUCKING BULLSHIT!!!
        F32 a = 0.5f * ball_a_acc->getLength();
        F32 discriminant = (F32)sqrt(square(v) - 4 * a * s);
        // TODO(annad): math, sqrt
        F32 t = f32Abs((-v + discriminant) / (2.0f * a));
        // F32 t = s / v;
        // NOTE(annad): Recalculate velocity after collision.
        ball_a->pos += ((*ball_a_acc) * 0.5f * square(t) + ball_a->vel * t);
        ball_a->vel += ((*ball_a_acc) * t);
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
        *dt = *dt - t;
        
        EvalPrintF(ball_a->vel.x);
        EvalPrintF(ball_a->vel.y);
        EvalPrintF(t);
    }
    printf("======================[  COLLISION HANDLE END  ]=========================\n\n\n\n");
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
            Ball *ball = &game_state->ball[i];
            ball->id = (Ball_Enum)i;
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
            game_state->ball[i].pos = {
                pos_x + renderer->context.width / 2,
                pos_y + renderer->context.height / 2,
            };
            game_state->ball[i].vel = {};
        }
        
        game_state->DEBUG_pause_game = false;
    }
    
    Vec2Dim<F32> ball_control_acc = {};
    
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
            Ball *ball_a = &game_state->ball[i];
            Vec2Dim<F32> ball_a_acc = {};
            if(i == BALL_ENUM_WHITE) 
            {
                ball_a_acc = ball_control_acc; // TODO(annad): Remove this.
            }
            
            ball_a_acc = ball_a_acc * BALL_SPEED + ball_a->vel * (-BALL_FRICTION);
            Ball updated_ball_a = update_ball(ball_a, &ball_a_acc, dt);
            for(S32 j = i + 1; j < BALL_ENUM_COUNT; j += 1)
            {
                if(i != j) 
                {
                    Ball *ball_b = &game_state->ball[j];
                    if(balls_is_collide(&updated_ball_a, ball_b))
                    {
                        Ball copy_ball_a = *ball_a;
                        Ball copy_ball_b = *ball_b;
                        F32 copy_dt = dt;
                        balls_collide_handle(&copy_ball_a, &copy_ball_b, &ball_a_acc, &copy_dt);
                        updated_ball_a = copy_ball_a;
                        *ball_b = copy_ball_b;
                    }
                }
            }
            *ball_a = updated_ball_a;
        }
    }
    
    Ball *white_ball = &game_state->ball[BALL_ENUM_WHITE];
    
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
        Ball *iter_ball = &game_state->ball[i];
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

