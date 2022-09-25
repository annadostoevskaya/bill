/* 
Author: github.com/annadostoevskaya
File: bill.cpp
Date: September 24th 2022 8:05 pm 

Description: <empty>
*/

#include "bill_math.h"
#include "bill.h"

void GameUpdateAndRender(GameMemory *game_memory, RendererCommands *renderer_commands, GameInput *game_input, GameTime *game_time)
{
    GameState *game_state = (GameState*)game_memory->permanent_storage;
    if(game_state->initialize_flag == false)
    {
        MemArena *memory_arena = &game_state->memory_arena;
        memory_arena->base = game_memory->persistent_storage;
        memory_arena->size = game_memory->persistent_storage_size;
        memory_arena->pos = 0;
        
        game_state->initialize_flag = true;
    }
    
    RendererCommands_push(renderer_commands, 
                          RENDERER_COMMAND_SET_RENDER_COLOR,
                          (U8)(game_input->mouse.cursor_pos.x), 
                          (U8)(game_input->mouse.cursor_pos.x), 
                          (U8)(game_input->mouse.cursor_pos.x),
                          (U8)(game_input->mouse.cursor_pos.x));
    
    RendererCommands_push(renderer_commands, 
                          RENDERER_COMMAND_DRAW_FILL_RECT, 
                          game_input->mouse.cursor_pos.x, 
                          game_input->mouse.cursor_pos.y, 
                          10, 10);
    
    EvalPrintB(isLittleEndian());
    
    EvalPrint(game_time->dt);
}

