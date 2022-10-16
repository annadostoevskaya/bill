/* 
Author: github.com/annadostoevskaya
File: bill.cpp
Date: September 24th 2022 8:05 pm 

Description: <empty>
*/

#include "bill_renderer.cpp"
#include "bill_math.h"
#include "bill.h"

template <typename T>
struct Matrix3x3
{
    T e[3][3];
};

void GameUpdateAndRender(GameMemory *game_memory, 
                         Renderer *renderer, 
                         GameInput *game_input, 
                         GameTime *game_time)
{
    (void)game_input;
    (void)game_time;
    
    Matrix3x3<S32> mtrx = {
        {
            {110, 110, 0}, 
            {120, 100,  0}, 
            {130,  90,  0}
        }
    };
    
    Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_LINE, 
                         mtrx.e[0][0], mtrx.e[0][1], 
                         mtrx.e[1][0], mtrx.e[1][1]);
    
    Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_POINT, 
                         mtrx.e[2][0], mtrx.e[2][1]);
    
    
    GameState *game_state = (GameState*)game_memory->permanent_storage;
    if(game_state->initialize_flag == false)
    {
        MemArena *memory_arena = &game_state->memory_arena;
        memory_arena->base = game_memory->persistent_storage;
        memory_arena->size = game_memory->persistent_storage_size;
        memory_arena->pos = 0;
        
        game_state->initialize_flag = true;
    }
    
    globalv Vec2Dim<F32> playerPos = {((F32)renderer->context.width / 2.0f), ((F32)renderer->context.height / 2.0f)};
    globalv Vec2Dim<F32> playerVel = {};
    Vec2Dim<F32> playerAcc = {};
    F32 playerSpeed = 2500.0f;
    
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
        playerPos = {};
    }
    
    // NOTE(annad):
    // x = f(t) = (a/2)t^2 + v0*t + x0;
    // v = x' = f(t)' = at + v0;
    // a = x'' = f(t)'' = a;
    
    // x0 += (a/2)t^2 + v0*t;
    // v0 += at;
    playerAcc *= playerSpeed; // m/s^2
    
    // TODO(annad): ODE!!!
    // x''(t) = -N * x'(t), N = m * omega
    playerAcc += (playerVel * (-3.0f));
    Vec2Dim<F32> newPlayerPos = {};
    F32 dt = (((F32)game_time->dt / 1000.0f));
    
    newPlayerPos = playerPos + (((playerAcc * 0.5f * square(dt))) + (playerVel * dt));
    playerVel += playerAcc * dt; 
    
    S32 playerWidth = 50;
    S32 playerHeight = 50;
    S32 collidePointX = (S32)(newPlayerPos.x) + (playerWidth / 2);
    S32 collidePointY = (S32)(newPlayerPos.y) + (playerHeight / 2);
    
    if(newPlayerPos.x > renderer->context.width - 50 || newPlayerPos.x < 0 ||
       newPlayerPos.y > renderer->context.height - 50 || newPlayerPos.y < 0)
    {
        // NOTE(annad):
        // I get the scalar multiplication of the velocity vector by the 
        // normal vector to the wall. Then I multiply the scalar by the 
        // wall normal vector to get the direction correction vector. After 
        // I subtract this vector from the velocity vector to correct the 
        // direction.
        
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
        EvalPrint(playerVel.x);
        EvalPrint(playerVel.y);
        
        newPlayerPos = playerPos + (((playerAcc * 0.5f * square(dt))) + (playerVel * dt));
    }
    
    playerPos = newPlayerPos;
    
    Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_LINE, 
                         0, 10, renderer->context.width, 10);
    Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_LINE, 
                         10, 0, 10, renderer->context.height);
    Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_LINE, 
                         0, renderer->context.height - 10, 
                         renderer->context.width, renderer->context.height - 10);
    Renderer_pushCommand(renderer, RENDERER_COMMAND_DRAW_LINE, 
                         renderer->context.width - 10, 0,
                         renderer->context.width - 10, renderer->context.height);
    
    // EvalPrintF(playerVel.getLength());
    
    RGBA_U8 color{0xff, 0x0, 0x0, 0xff};
    RGBA_U8 color2{0x00, 0xaa, 0x0, 0xff};
    Renderer_pushCommand(renderer, 
                         RENDERER_COMMAND_SET_RENDER_COLOR,
                         &color);
    
    S32 playerPosXOnScreen = (S32)(playerPos.x);
    S32 playerPosYOnScreen = (S32)(playerPos.y);
    
    Rect rect{playerPosXOnScreen, playerPosYOnScreen, playerWidth, playerHeight};
    Rect rect2{collidePointX, collidePointY, 3, 3};
    Renderer_pushCommand(renderer, 
                         RENDERER_COMMAND_DRAW_FILL_RECT, 
                         &rect);
    
    Renderer_pushCommand(renderer, 
                         RENDERER_COMMAND_SET_RENDER_COLOR,
                         &color2);
    
    Renderer_pushCommand(renderer, 
                         RENDERER_COMMAND_DRAW_FILL_RECT, 
                         &rect2);
}

