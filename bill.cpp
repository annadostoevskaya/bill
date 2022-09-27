/* 
Author: github.com/annadostoevskaya
File: bill.cpp
Date: September 24th 2022 8:05 pm 

Description: <empty>
*/

#include "bill_math.h"
#include "bill.h"

void GameUpdateAndRender(GameMemory *game_memory, Renderer *renderer, GameInput *game_input, GameTime *game_time)
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
    }
    
    RGBA_U8 color;
    color.r = 0xff;
    color.g = 0x00;
    color.b = 0x00;
    color.a = 0xff;
    Renderer_pushCommand(renderer, 
                         RENDERER_COMMAND_SET_RENDER_COLOR,
                         &color);
    
    Rect rect;
    rect.x = game_input->mouse.cursor_pos.x;
    rect.y = game_input->mouse.cursor_pos.y;
    rect.w = 100;
    rect.h = 100;
    
    Renderer_pushCommand(renderer, 
                         RENDERER_COMMAND_DRAW_FILL_RECT, 
                         &rect);
    
    
    color.r = 0x00;
    color.g = 0x00;
    color.b = 0xff;
    color.a = 0xff;
    Renderer_pushCommand(renderer, 
                         RENDERER_COMMAND_SET_RENDER_COLOR,
                         &color);
    
    
    rect.x = game_input->mouse.cursor_pos.y;
    rect.y = game_input->mouse.cursor_pos.x;
    rect.w = 100;
    rect.h = 100;
    
    Renderer_pushCommand(renderer, 
                         RENDERER_COMMAND_DRAW_FILL_RECT, 
                         &rect);
    
    
    color.r = 0x00;
    color.g = 0xff;
    color.b = 0x00;
    color.a = 0xff;
    Renderer_pushCommand(renderer, 
                         RENDERER_COMMAND_SET_RENDER_COLOR,
                         &color);
    
    rect.x = (S32)sqrt(game_input->mouse.cursor_pos.y * game_input->mouse.cursor_pos.y + game_input->mouse.cursor_pos.x * game_input->mouse.cursor_pos.x);
    rect.y = rect.x;
    rect.w = 100;
    rect.h = 100;
    
    Renderer_pushCommand(renderer, 
                         RENDERER_COMMAND_DRAW_FILL_RECT, 
                         &rect);
}

