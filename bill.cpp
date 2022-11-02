/* 
Author: github.com/annadostoevskaya
File: bill.cpp
Date: September 24th 2022 8:05 pm 

Description: <empty>
*/

#include "bill_renderer.cpp"
#include "bill_math.h"
#include "bill.h"

void game_update_and_render(GameMemory *game_memory, 
                            Renderer *renderer, 
                            GameInput *game_input, 
                            GameTime *game_time)
{
    (void)game_input;
    (void)game_time;
    
    GameState *game_state = (GameState*)game_memory->permanent_storage;
    if(game_state->initialize_flag == false)
    {
        MemArena *memory_arena = &game_state->memory_arena;
        memory_arena->base = game_memory->persistent_storage;
        memory_arena->size = game_memory->persistent_storage_size;
        memory_arena->pos = 0;
        
        game_state->initialize_flag = true;
        game_state->DEBUG_pause_game = false;
    }
    
    localv Vec2Dim<F32> ballNumber2Pos = {((F32)renderer->context.width / 2.0f), ((F32)renderer->context.height / 2.0f) - 150};
    localv Vec2Dim<F32> ballNumber2Vel = {};
    Vec2Dim<F32> ballNumber2Acc = {};
    
    localv Vec2Dim<F32> DEBUG_playerDirection = {};
    localv Vec2Dim<F32> DEBUG_ballDirection = {};
    localv Vec2Dim<F32> playerPos = {((F32)renderer->context.width / 2.0f), ((F32)renderer->context.height / 2.0f)};
    localv Vec2Dim<F32> playerVel = {};
    localv F32 deltaD = 0.0f;
    Vec2Dim<F32> playerAcc = {};
    F32 playerSpeed = 2500.0f;
    
    S32 constBallRadius = 50;
    
    if(game_input->keyboard.keys[INPUT_KEYBOARD_KEYS_RETURN])
    {
        game_state->DEBUG_pause_game = false;
        ballNumber2Pos = {((F32)renderer->context.width / 2.0f), ((F32)renderer->context.height / 2.0f) - 150.0f};
        playerPos = {((F32)renderer->context.width / 2.0f), ((F32)renderer->context.height / 2.0f)};
        playerVel = {};
        ballNumber2Vel = {};
    }
    
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
        playerAcc += (playerVel * (-3.0f));
        Vec2Dim<F32> newPlayerPos = {};
        Vec2Dim<F32> newPlayerVel = {};
        F32 dt = (((F32)game_time->dt / 1000.0f));
        
        newPlayerPos = playerPos + (((playerAcc * 0.5f * square(dt))) + 
                                    (playerVel * dt));
        newPlayerVel = playerVel + playerAcc * dt;
        
        ballNumber2Acc += (ballNumber2Vel * (-3.0f));
        ballNumber2Pos += (((ballNumber2Acc * 0.5f * square(dt))) + 
                           (ballNumber2Vel * dt));
        ballNumber2Vel += ballNumber2Acc * dt;
        
        Vec2Dim<F32> temp = newPlayerPos - ballNumber2Pos;
        if(temp.getLength() < (2.0f * (F32)constBallRadius))
        {
            // IMPORTANT(Saiel): You suck, rewrite this.
            
            
            // TODO(annad): Use t = (-v + (F32)sqrt(square(v) - (4*a*s))) / (2.0f * a)!!!
            // NOTE(annad): Recalc position!
            Vec2Dim<F32> S = ballNumber2Pos - playerPos;
            F32 cos = playerVel.innerProduct(S) / (S.getLength() * playerVel.getLength());
            F32 v = playerVel.getLength() * cos;
            F32 s = S.getLength() - 2.0f * (F32)constBallRadius;
            F32 a = 0.5f * playerAcc.getLength();
            // F32 t = (-v + (F32)sqrt(square(v) - (4*a*s))) / (2.0f * a);
            F32 D = (F32)sqrt(square(v) - (4*a*(s)));
            F32 t2 = f32Abs((-v + D) / (2.0f * a)); (void)t2;
            F32 t = s / v;
            // __debugbreak();
            // newPlayerPos = playerPos + (playerVel * t);
            
            newPlayerPos = playerPos + (((playerAcc * 0.5f * square(t))) + 
                                        (playerVel * t));
            newPlayerVel = playerVel + playerAcc * t;
            
            // newPlayerVel = playerVel + playerAcc * t;
            
            // NOTE(annad): Recalc of Velocity!
            // __debugbreak();
            Vec2Dim<F32> directV1 = {S.x / S.getLength(), S.y / S.getLength()};
            Vec2Dim<F32> directV2 = {directV1.y, -directV1.x};
            
            DEBUG_ballDirection = directV1;
            DEBUG_playerDirection = directV2;
            
            F32 cos_v1 = newPlayerVel.innerProduct(directV1) / (newPlayerVel.getLength() * directV1.getLength());
            F32 v1 = newPlayerVel.getLength() * cos_v1;
            
            F32 cos_v2 = newPlayerVel.innerProduct(directV2) / (directV2.getLength() * newPlayerVel.getLength());
            F32 v2 = newPlayerVel.getLength() * cos_v2;
            
            ballNumber2Vel = directV1 * v1;
            newPlayerVel = (directV2) * v2;
            
            // game_state->DEBUG_pause_game = true;
        }
        
        playerVel = newPlayerVel;
        
        if(newPlayerPos.x > renderer->context.width - 50 || newPlayerPos.x < 0 ||
           newPlayerPos.y > renderer->context.height - 50 || newPlayerPos.y < 0)
        {
            // NOTE(annad):
            // I get the scalar multiplication of the velocity vector by the 
            // normal vector to the wall. Then I multiply the scalar by the 
            // wall normal vector to get the direction correction vector. After 
            // I subtract this vector from the velocity vector to correct the 
            // direction.
            
            /*             
                        Vec2Dim<F32> wall = {};
                        
                        if(newPlayerPos.x < 0)
                        {
                            wall = {1.0f, 0.0f};
                        }
                        
                        if(newPlayerPos.x > renderer->context.width - 50)
                        {
                            wall = {-1.0f, 0.0f};
                        }
                        
                        if(newPlayerPos.y < 0)
                        {
                            wall = {0.0f, 1.0f};
                        }
                        
                        if(newPlayerPos.y > renderer->context.height - 50)
                        {
                            wall = {0.0f, -1.0f};
                        }
                        
                        playerVel -= wall * (2.0f * playerVel.innerProduct(wall));
                        
                        newPlayerPos = playerPos + (((playerAcc * 0.5f * square(dt))) + (playerVel * dt));
                         */
            
        }
        playerPos = newPlayerPos;
    }
    
    RGBA_U8 color{0xff, 0x0, 0x0, 0xff};
    renderer_push_command(renderer, 
                          RENDERER_COMMAND_SET_RENDER_COLOR,
                          &color);
    
    renderer_push_command(renderer, RENDERER_COMMAND_DRAW_CIRCLE, 
                          (S32)f32Round(playerPos.x), 
                          (S32)f32Round(playerPos.y), 
                          constBallRadius);
    
    RGBA_U8 color2{0x00, 0xff, 0x0, 0xff};
    renderer_push_command(renderer, 
                          RENDERER_COMMAND_SET_RENDER_COLOR,
                          &color2);
    renderer_push_command(renderer, RENDERER_COMMAND_DRAW_CIRCLE, 
                          (S32)ballNumber2Pos.x, 
                          (S32)ballNumber2Pos.y, 
                          constBallRadius);
    
    RGBA_U8 color3{100, 50, 130, 0xff};
    renderer_push_command(renderer, 
                          RENDERER_COMMAND_SET_RENDER_COLOR,
                          &color3);
    
    renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                          (S32)playerPos.x, 
                          (S32)playerPos.y, 
                          (S32)(playerPos.x + playerVel.x), 
                          (S32)(playerPos.y + playerVel.y));
    
    renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                          (S32)ballNumber2Pos.x, 
                          (S32)ballNumber2Pos.y, 
                          (S32)(ballNumber2Pos.x + ballNumber2Vel.x), 
                          (S32)(ballNumber2Pos.y + ballNumber2Vel.y));
    
    RGBA_U8 color5{20, 20, 255, 0xff};
    renderer_push_command(renderer, 
                          RENDERER_COMMAND_SET_RENDER_COLOR,
                          &color5);
    
    renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                          (S32)ballNumber2Pos.x, 
                          (S32)ballNumber2Pos.y, 
                          (S32)(ballNumber2Pos.x + 100 * DEBUG_ballDirection.x), 
                          (S32)(ballNumber2Pos.y + 100 * DEBUG_ballDirection.y));
    
    renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE,
                          (S32)playerPos.x, 
                          (S32)playerPos.y, 
                          (S32)(playerPos.x + 100 * DEBUG_playerDirection.x), 
                          (S32)(playerPos.y + 100 * DEBUG_playerDirection.y));
    
    if(game_state->DEBUG_pause_game)
    {
        RGBA_U8 color4{0xff, 0xff, 0xff, 0xff};
        renderer_push_command(renderer, 
                              RENDERER_COMMAND_SET_RENDER_COLOR,
                              &color4);
        
        renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                              (S32)playerPos.x, 
                              (S32)playerPos.y, 
                              (S32)(ballNumber2Pos.x), 
                              (S32)(ballNumber2Pos.y));
        
        if(f32Abs(playerVel.x) >= f32Abs(playerVel.y))
        {
            renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                                  (S32)ballNumber2Pos.x, 
                                  (S32)ballNumber2Pos.y + (S32)(deltaD), 
                                  (S32)ballNumber2Pos.x, 
                                  (S32)ballNumber2Pos.y);
            
            
            renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                                  (S32)playerPos.x, 
                                  (S32)playerPos.y, 
                                  (S32)ballNumber2Pos.x, 
                                  (S32)ballNumber2Pos.y + (S32)(deltaD));
        }
        else
        {
            renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                                  (S32)ballNumber2Pos.x + (S32)(deltaD), 
                                  (S32)ballNumber2Pos.y, 
                                  (S32)ballNumber2Pos.x, 
                                  (S32)ballNumber2Pos.y);
            
            renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                                  (S32)playerPos.x, 
                                  (S32)playerPos.y, 
                                  (S32)ballNumber2Pos.x + (S32)(deltaD), 
                                  (S32)ballNumber2Pos.y);
        }
        
        renderer_push_command(renderer, 
                              RENDERER_COMMAND_SET_RENDER_COLOR,
                              &color5);
        
        renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                              (S32)ballNumber2Pos.x, 
                              (S32)ballNumber2Pos.y, 
                              (S32)(ballNumber2Pos.x + 100 * DEBUG_ballDirection.x), 
                              (S32)(ballNumber2Pos.y + 100 * DEBUG_ballDirection.y));
        
        renderer_push_command(renderer, RENDERER_COMMAND_DRAW_LINE, 
                              (S32)playerPos.x, 
                              (S32)playerPos.y, 
                              (S32)(playerPos.x + 100 * DEBUG_playerDirection.x), 
                              (S32)(playerPos.y + 100 * DEBUG_playerDirection.y));
    }
}

