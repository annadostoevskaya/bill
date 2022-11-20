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
Ball update_ball(Ball *ball, Vec2Dim<F32> acc, F32 dt)
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
    acc = acc * BALL_SPEED + ball->vel * (-BALL_FRICTION);
    Ball updated_ball;
    updated_ball.pos = ball->pos + acc * 0.5f * square(dt) + ball->vel * dt;
    updated_ball.vel = ball->vel + acc * dt;
    return updated_ball;
}

// TODO(annad): Refactoring...
B32 balls_is_collide(Vec2Dim<F32> *ball_a_pos, 
                     Vec2Dim<F32> *ball_b_pos, 
                     F32 radius)
{
    F32 distance = (*ball_a_pos - *ball_b_pos).getLength();
    return distance < (2.0f * radius);
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
            Ball *ball = &game_state->ball[i];
            Vec2Dim<F32> ball_acc = {};
            if(i == BALL_ENUM_WHITE) 
            {
                ball_acc = ball_control_acc; // TODO(annad): Remove this.
            }
            Ball updated_ball = update_ball(ball, ball_acc, dt);
            
            for(S32 j = 0; j < BALL_ENUM_COUNT; j += 1)
            {
                if(i == j) 
                {
                    break;
                }
                
                Ball *ball_b = &game_state->ball[j];
                if(balls_is_collide(&updated_ball.pos, &ball_b->pos, (F32)const_ball_radius))
                {
                    EvalPrint(i);
                }
            }
            
            *ball = updated_ball;
            
            /*             
                        if(balls_is_collide(&newPlayerPos, &ball->pos, (F32)const_ball_radius))
                        {
                            // IMPORTANT(Saiel): You suck, rewrite this.
                            // TODO(annad): Use t = (-v + (F32)sqrt(square(v) - (4*a*s))) / (2.0f * a)!!!
                            // NOTE(annad): Recalc position!
                            Vec2Dim<F32> S = ball->pos - white_ball->pos;
                            F32 cos = white_ball->vel.innerProduct(S) / (S.getLength() * white_ball->vel.getLength());
                            F32 v = white_ball->vel.getLength() * cos;
                            F32 s = S.getLength() - 2.0f * (F32)const_ball_radius;
                            F32 a = 0.5f * ball_control_acc.getLength();
                            F32 D = (F32)sqrt(square(v) - (4*a*(s)));
                            F32 t = f32Abs((-v + D) / (2.0f * a));
                            // newPlayerPos = playerPos + (white_ball->vel * t);
                            newPlayerPos = white_ball->pos + (((ball_control_acc * 0.5f * square(t))) + 
                                                              (white_ball->vel * t));
                            newPlayerVel = white_ball->vel + ball_control_acc * t;
                            // newPlayerVel = white_ball->vel + ball_control_acc * t;
                            // NOTE(annad): Recalc of Velocity!
                            Vec2Dim<F32> directV1 = {S.x / S.getLength(), S.y / S.getLength()};
                            Vec2Dim<F32> directV2 = {directV1.y, -directV1.x};
                            
                            DEBUG_ballDirection = directV1;
                            DEBUG_playerDirection = directV2;
                            
                            F32 cos_v1 = newPlayerVel.innerProduct(directV1) / (newPlayerVel.getLength() * directV1.getLength());
                            F32 v1 = newPlayerVel.getLength() * cos_v1;
                            
                            F32 cos_v2 = newPlayerVel.innerProduct(directV2) / (directV2.getLength() * newPlayerVel.getLength());
                            F32 v2 = newPlayerVel.getLength() * cos_v2;
                            
                            ball->vel = directV1 * v1;
                            newPlayerVel = (directV2) * v2;
                        }
                         */
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

