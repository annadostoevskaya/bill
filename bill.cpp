/* 
Author: github.com/annadostoevskaya
File: bill.cpp
Date: September 24th 2022 8:05 pm 

Description: <empty>
*/

#include "bill_renderer.cpp"
#include "bill_math.h"
#include "bill.h"

typedef struct Circle
{
    S32 r;
    Point2Dim<S32> pos;
} Circle;

void GameUpdateAndRender(GameMemory *game_memory, 
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
        game_state->debugPauseGame = false;
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
        game_state->debugPauseGame = false;
        ballNumber2Pos = {((F32)renderer->context.width / 2.0f), ((F32)renderer->context.height / 2.0f) - 150.0f};
        ballNumber2Vel = {};
        playerPos = {((F32)renderer->context.width / 2.0f), ((F32)renderer->context.height / 2.0f)};
        playerVel = {};
    }
    
    if(!game_state->debugPauseGame)
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
        
        if(game_input->keyboard.keys[INPUT_KEYBOARD_KEYS_RETURN])
        {
            game_state->debugPauseGame = false;
            playerPos = {};
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
        if(temp.getLength() <= (2.0f * (F32)constBallRadius))
        {
            printf("[ COLLIDE ]\n");
            
            F64 s = (playerPos - ballNumber2Pos).getLength() - (2 * constBallRadius);
            F64 v = playerVel.getLength();
            F64 t = s / v;
            
            EvalPrintF(s);
            EvalPrintF(v);
            EvalPrintF(t);
            
            newPlayerPos = playerPos + playerVel * t; // ik
            
            Vec2Dim<F32> tempPos = newPlayerPos;
            temp = tempPos - ballNumber2Pos;
            S32 collideTryes = 0;
            while((temp.getLength() > (2.0f * (F32)constBallRadius)) && (collideTryes < 4))
            {
                printf("[ DID'T COLLIDE ]\n");
                
                EvalPrintF(s);
                EvalPrintF(v);
                EvalPrintF(t);
                
                s = (tempPos - ballNumber2Pos).getLength() - (2 * constBallRadius);
                v = playerVel.getLength();
                t = s / v;
                
                newPlayerPos = tempPos + playerVel * t;
                
                tempPos = newPlayerPos;
                temp = tempPos - ballNumber2Pos;
                collideTryes++;
            }
            
            if((temp.getLength() > (2.0f * (F32)constBallRadius)))
            {
                printf("[ DID'T COLLIDE ]\n");
            }
            
            // find new VELOCITY and new DIRECTION!
            // the distance between the balls on the projection 
            // to the normal of the vector between the balls.
            if(f32Abs(playerVel.x) >= f32Abs(playerVel.y))
            {
                deltaD = (newPlayerPos.y - ballNumber2Pos.y);
            }
            else
            {
                deltaD = (newPlayerPos.x - ballNumber2Pos.x);
            }
            
            if(deltaD == 0.0f)
            {
                ballNumber2Vel = newPlayerVel;
                ballNumber2Acc = playerAcc;
                newPlayerVel = {0.0f, 0.0f};
            }
            else
            {
                F32 angleInRadians = f32Abs(deltaD) / ((F32)2*constBallRadius);
                F32 angleTurn = ((angleInRadians + PI_F32 / 2.0f) / PI_F32);
                EvalPrintF(deltaD);
                EvalPrintF(angleTurn);
                
                F32 angleDegrees = angleTurn * 180.0f - 180.0f;
                EvalPrintF(angleDegrees);
                
                Vec2Dim<F32> delta = (newPlayerPos - ballNumber2Pos);
                Vec2Dim<F32> directionBall = {f32Abs(delta.x) / f32Abs(delta.getLength()), 
                    f32Abs(delta.y) / f32Abs(delta.getLength())};
                
                F32 ballLinerarCoefficient = (f32Abs(deltaD) / (F32)constBallRadius);
                ballNumber2Vel = directionBall * ballLinerarCoefficient * playerVel.getLength();
                ballNumber2Acc = playerAcc;
                
                //              |x|
                //              |y|
                // | cos0 sin0|  x*cos0 + x*sin0
                // |-sin0 cos0| -y*sin0 + y*cos0
                
                Vec2Dim<F32> playerDirection = {
                    directionBall.x, -directionBall.y
                        // directionBall.x * turnCos(angleTurn) + directionBall.x * turnSin(angleTurn),
                        // directionBall.y * (-turnSin(angleTurn)) + directionBall.y * turnCos(angleTurn),
                };
                
                DEBUG_playerDirection = playerDirection;
                DEBUG_ballDirection = directionBall;
                
                F32 playerLinerarCoefficient = (1.0f - ballLinerarCoefficient);
                newPlayerVel = playerDirection * playerLinerarCoefficient * playerVel.getLength();
                newPlayerVel += (playerAcc * dt);
            }
            // game_state->debugPauseGame = true;
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
    Renderer_pushCommand(renderer, 
                         RENDERER_COMMAND_SET_RENDER_COLOR,
                         &color);
    
    Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_CIRCLE, 
                         (S32)f32Round(playerPos.x), 
                         (S32)f32Round(playerPos.y), 
                         constBallRadius);
    
    RGBA_U8 color2{0x00, 0xff, 0x0, 0xff};
    Renderer_pushCommand(renderer, 
                         RENDERER_COMMAND_SET_RENDER_COLOR,
                         &color2);
    Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_CIRCLE, 
                         (S32)ballNumber2Pos.x, 
                         (S32)ballNumber2Pos.y, 
                         constBallRadius);
    
    RGBA_U8 color3{100, 50, 130, 0xff};
    Renderer_pushCommand(renderer, 
                         RENDERER_COMMAND_SET_RENDER_COLOR,
                         &color3);
    
    Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_LINE, 
                         (S32)playerPos.x, 
                         (S32)playerPos.y, 
                         (S32)(playerPos.x + playerVel.x), 
                         (S32)(playerPos.y + playerVel.y));
    
    Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_LINE, 
                         (S32)ballNumber2Pos.x, 
                         (S32)ballNumber2Pos.y, 
                         (S32)(ballNumber2Pos.x + ballNumber2Vel.x), 
                         (S32)(ballNumber2Pos.y + ballNumber2Vel.y));
    
    
    RGBA_U8 color5{20, 20, 255, 0xff};
    Renderer_pushCommand(renderer, 
                         RENDERER_COMMAND_SET_RENDER_COLOR,
                         &color5);
    
    Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_LINE, 
                         (S32)ballNumber2Pos.x, 
                         (S32)ballNumber2Pos.y, 
                         (S32)(ballNumber2Pos.x + 100 * DEBUG_ballDirection.x), 
                         (S32)(ballNumber2Pos.y + 100 * DEBUG_ballDirection.y));
    
    Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_LINE, 
                         (S32)playerPos.x, 
                         (S32)playerPos.y, 
                         (S32)(playerPos.x + 100 * DEBUG_playerDirection.x), 
                         (S32)(playerPos.y + 100 * DEBUG_playerDirection.y));
    
    if(game_state->debugPauseGame)
    {
        RGBA_U8 color4{0xff, 0xff, 0xff, 0xff};
        Renderer_pushCommand(renderer, 
                             RENDERER_COMMAND_SET_RENDER_COLOR,
                             &color4);
        
        Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_LINE, 
                             (S32)playerPos.x, 
                             (S32)playerPos.y, 
                             (S32)(ballNumber2Pos.x), 
                             (S32)(ballNumber2Pos.y));
        
        if(f32Abs(playerVel.x) >= f32Abs(playerVel.y))
        {
            Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_LINE, 
                                 (S32)ballNumber2Pos.x, 
                                 (S32)ballNumber2Pos.y + (S32)(deltaD), 
                                 (S32)ballNumber2Pos.x, 
                                 (S32)ballNumber2Pos.y);
            
            
            Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_LINE, 
                                 (S32)playerPos.x, 
                                 (S32)playerPos.y, 
                                 (S32)ballNumber2Pos.x, 
                                 (S32)ballNumber2Pos.y + (S32)(deltaD));
        }
        else
        {
            Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_LINE, 
                                 (S32)ballNumber2Pos.x + (S32)(deltaD), 
                                 (S32)ballNumber2Pos.y, 
                                 (S32)ballNumber2Pos.x, 
                                 (S32)ballNumber2Pos.y);
            
            Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_LINE, 
                                 (S32)playerPos.x, 
                                 (S32)playerPos.y, 
                                 (S32)ballNumber2Pos.x + (S32)(deltaD), 
                                 (S32)ballNumber2Pos.y);
            
        }
        
        
        Renderer_pushCommand(renderer, 
                             RENDERER_COMMAND_SET_RENDER_COLOR,
                             &color5);
        
        Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_LINE, 
                             (S32)ballNumber2Pos.x, 
                             (S32)ballNumber2Pos.y, 
                             (S32)(ballNumber2Pos.x + 100 * DEBUG_ballDirection.x), 
                             (S32)(ballNumber2Pos.y + 100 * DEBUG_ballDirection.y));
        
        Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_LINE, 
                             (S32)playerPos.x, 
                             (S32)playerPos.y, 
                             (S32)(playerPos.x + 100 * DEBUG_playerDirection.x), 
                             (S32)(playerPos.y + 100 * DEBUG_playerDirection.y));
    }
}

