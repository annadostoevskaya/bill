/* 
Author: github.com/annadostoevskaya

File: bill.cpp
Date: September 24th 2022 8:05 pm 
Description: <empty>
*/

#include "bill_renderer.cpp"
#include "bill_math.h"
#include "bill.h"

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
    
    Ball *white_ball = &game_state->ball[BALL_ENUM_WHITE];
    Ball *ball = &game_state->ball[BALL_ENUM_2];
    
    // MemArena *memory_arena = &game_state->memory_arena;
    
    Vec2Dim<F32> ballNumber2Acc = {};
    
    localv Vec2Dim<F32> DEBUG_playerDirection = {};
    localv Vec2Dim<F32> DEBUG_ballDirection = {};
    
    localv F32 deltaD = 0.0f;
    
    F32 playerSpeed = 2500.0f;
    
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
    
    Vec2Dim<F32> playerAcc = {};
    
    if(!game_state->DEBUG_pause_game)
    {
        if(game_input->keyboard.keys[INPUT_KEYBOARD_KEYS_S])
        {
            playerAcc.y = 1.0f;
        }
        
        if(game_input->keyboard.keys[INPUT_KEYBOARD_KEYS_W])
        {
            playerAcc.y = -1.0f;
        }
        
        if(game_input->keyboard.keys[INPUT_KEYBOARD_KEYS_A])
        {
            playerAcc.x = -1.0f;
        }
        
        if(game_input->keyboard.keys[INPUT_KEYBOARD_KEYS_D])
        {
            playerAcc.x = 1.0f;
        }
        
        if((playerAcc.x != 0.0f) && (playerAcc.y != 0.0f))
        {
            playerAcc *= 0.70710678118f;
        }
        
        // NOTE(annad):
        // x = f(t) = (a/2)t^2 + v0*t + x0;
        // v = x' = f(t)' = at + v0;
        // a = x'' = f(t)'' = a;
        
        // x0 += (a/2)t^2 + v0*t;
        // v0 += at;
        playerAcc *= playerSpeed; // m/s^2
        
        // TODO(annad): ODE!!
        // TODO(annad): We need to find coef. of 
        // friction of the billiard ball on the table.
        // x''(t) = -N * x'(t), N = m * omega
        playerAcc += (white_ball->vel * (-3.0f));
        Vec2Dim<F32> newPlayerPos = {};
        Vec2Dim<F32> newPlayerVel = {};
        F32 dt = (((F32)game_time->dt / 1000.0f));
        
        newPlayerPos = white_ball->pos + (((playerAcc * 0.5f * square(dt))) + 
                                          (white_ball->vel * dt));
        newPlayerVel = white_ball->vel + playerAcc * dt;
        
        for(S32 j = BALL_ENUM_2; j < BALL_ENUM_COUNT; j += 1)
        {
            ball = &game_state->ball[j];
            ballNumber2Acc = {};
            ballNumber2Acc += (ball->vel * (-3.0f));
            ball->pos += (((ballNumber2Acc * 0.5f * square(dt))) + 
                          (ball->vel * dt));
            ball->vel += ballNumber2Acc * dt;
            
            if(balls_is_collide(&newPlayerPos, &ball->pos, (F32)const_ball_radius))
            {
                // IMPORTANT(Saiel): You suck, rewrite this.
                // TODO(annad): Use t = (-v + (F32)sqrt(square(v) - (4*a*s))) / (2.0f * a)!!!
                // NOTE(annad): Recalc position!
                Vec2Dim<F32> S = ball->pos - white_ball->pos;
                F32 cos = white_ball->vel.innerProduct(S) / (S.getLength() * white_ball->vel.getLength());
                F32 v = white_ball->vel.getLength() * cos;
                F32 s = S.getLength() - 2.0f * (F32)const_ball_radius;
                F32 a = 0.5f * playerAcc.getLength();
                F32 D = (F32)sqrt(square(v) - (4*a*(s)));
                F32 t = f32Abs((-v + D) / (2.0f * a));
                // newPlayerPos = playerPos + (white_ball->vel * t);
                newPlayerPos = white_ball->pos + (((playerAcc * 0.5f * square(t))) + 
                                                  (white_ball->vel * t));
                newPlayerVel = white_ball->vel + playerAcc * t;
                // newPlayerVel = white_ball->vel + playerAcc * t;
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
        }
        
        white_ball->vel = newPlayerVel;
        white_ball->pos = newPlayerPos;
    }
    
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
            renderer_push_command(renderer, 
                                  RENDERER_COMMAND_SET_RENDER_COLOR,
                                  &c);
            
        }
        else
        {
            RGBA_U8 c = {0x00, 0xff, 0x0, 0xff};
            renderer_push_command(renderer, 
                                  RENDERER_COMMAND_SET_RENDER_COLOR,
                                  &c);
        }
        renderer_push_command(renderer, RENDERER_COMMAND_DRAW_CIRCLE, 
                              (S32)iter_ball->pos.x, 
                              (S32)iter_ball->pos.y, 
                              const_ball_radius);
    }
    
    RGBA_U8 color3{100, 50, 130, 0xff};
    renderer_push_command(renderer, 
                          RENDERER_COMMAND_SET_RENDER_COLOR,
                          &color3);
    
    renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                          (S32)white_ball->pos.x, 
                          (S32)white_ball->pos.y, 
                          (S32)(white_ball->pos.x + white_ball->vel.x), 
                          (S32)(white_ball->pos.y + white_ball->vel.y));
    
    renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                          (S32)ball->pos.x, 
                          (S32)ball->pos.y, 
                          (S32)(ball->pos.x + ball->vel.x), 
                          (S32)(ball->pos.y + ball->vel.y));
    
    RGBA_U8 color5{20, 20, 255, 0xff};
    renderer_push_command(renderer, 
                          RENDERER_COMMAND_SET_RENDER_COLOR,
                          &color5);
    
    renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                          (S32)ball->pos.x, 
                          (S32)ball->pos.y, 
                          (S32)(ball->pos.x + 100 * DEBUG_ballDirection.x), 
                          (S32)(ball->pos.y + 100 * DEBUG_ballDirection.y));
    
    renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE,
                          (S32)white_ball->pos.x, 
                          (S32)white_ball->pos.y, 
                          (S32)(white_ball->pos.x + 100 * DEBUG_playerDirection.x), 
                          (S32)(white_ball->pos.y + 100 * DEBUG_playerDirection.y));
    
    if(game_state->DEBUG_pause_game)
    {
        RGBA_U8 color4{0xff, 0xff, 0xff, 0xff};
        renderer_push_command(renderer, 
                              RENDERER_COMMAND_SET_RENDER_COLOR,
                              &color4);
        
        renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                              (S32)white_ball->pos.x, 
                              (S32)white_ball->pos.y, 
                              (S32)(ball->pos.x), 
                              (S32)(ball->pos.y));
        
        if(f32Abs(white_ball->vel.x) >= f32Abs(white_ball->vel.y))
        {
            renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                                  (S32)ball->pos.x, 
                                  (S32)ball->pos.y + (S32)(deltaD), 
                                  (S32)ball->pos.x, 
                                  (S32)ball->pos.y);
            
            
            renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                                  (S32)white_ball->pos.x, 
                                  (S32)white_ball->pos.y, 
                                  (S32)ball->pos.x, 
                                  (S32)ball->pos.y + (S32)(deltaD));
        }
        else
        {
            renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                                  (S32)ball->pos.x + (S32)(deltaD), 
                                  (S32)ball->pos.y, 
                                  (S32)ball->pos.x, 
                                  (S32)ball->pos.y);
            
            renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                                  (S32)white_ball->pos.x, 
                                  (S32)white_ball->pos.y, 
                                  (S32)ball->pos.x + (S32)(deltaD), 
                                  (S32)ball->pos.y);
        }
        
        renderer_push_command(renderer, 
                              RENDERER_COMMAND_SET_RENDER_COLOR,
                              &color5);
        
        renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                              (S32)ball->pos.x, 
                              (S32)ball->pos.y, 
                              (S32)(ball->pos.x + 100 * DEBUG_ballDirection.x), 
                              (S32)(ball->pos.y + 100 * DEBUG_ballDirection.y));
        
        renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                              (S32)white_ball->pos.x, 
                              (S32)white_ball->pos.y, 
                              (S32)(white_ball->pos.x + 100 * DEBUG_playerDirection.x), 
                              (S32)(white_ball->pos.y + 100 * DEBUG_playerDirection.y));
    }
}

