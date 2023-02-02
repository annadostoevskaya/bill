/* 
Author: github.com/annadostoevskaya
File: bill_debug.cpp
Date: December 16th 2022 11:47 pm 

Description: <empty>
*/

globalv GameIO          *dbg_GameIO;
globalv SDL_Window      *dbg_Window;
globalv SDL_Renderer    *dbg_SdlRenderer;
globalv RendererHandle  *dbg_HRenderer;
globalv S32             dbg_GlobalFrameCounter;

#define DbgPrint(STR, ...) printf("[dbg] " STR, __VA_ARGS__)

/*
globalv GameState *DEBUG_game_state;
globalv SDL_Renderer    *DEBUG_sdl_renderer;
globalv SDL_Renderer    *sdl_renderer;
globalv Renderer        *DEBUG_renderer;
globalv Vec2Dim<F32> DEBUG_DIRECTION_BALL_A;
globalv Vec2Dim<F32> DEBUG_DIRECTION_BALL_B;
globalv S32 DEBUG_DIRECTION_BALL_A_ID;
globalv S32 DEBUG_DIRECTION_BALL_B_ID;
*/
// void dbg_ForceUpdateScreen()

/*
void FORCE_UPDATE()
{
    for(S32 i = 0; i < BALL_COUNT; i++)
    {
        Ball *iter_ball = &DEBUG_game_state->balls[i];
        if(i == BALL_WHITE)
        {
            RGBA_U8 c = {0xff, 0xff, 0xff, 0xff};
            renderer_push_command(dbg_Renderer, RENDERER_COMMAND_SET_RENDER_COLOR, &c);
        }
        else
        {
            RGBA_U8 c = {0x00, 0xff, 0x0, 0xff};
            renderer_push_command(dbg_Renderer, RENDERER_COMMAND_SET_RENDER_COLOR, &c);
        }
        
        renderer_push_command(dbg_Renderer, RENDERER_COMMAND_DRAW_CIRCLE,
                              (S32)iter_ball->pos.x,
                              (S32)iter_ball->pos.y,
                              20);
    }
    
    RGBA_U8 cue_color = {0xff, 0x00, 0xff, 0xff};
    renderer_push_command(dbg_Renderer, RENDERER_COMMAND_SET_RENDER_COLOR, &cue_color);
    renderer_push_command(dbg_Renderer, RENDERER_COMMAND_DRAW_LINE,
                          (S32)DEBUG_game_state->balls[0].pos.x,
                          (S32)DEBUG_game_state->balls[0].pos.y,
                          (S32)DEBUG_game_state->balls[0].pos.x + ((S32)DEBUG_game_state->bill_cue.x),
                          (S32)DEBUG_game_state->balls[0].pos.y + ((S32)DEBUG_game_state->bill_cue.y));
    
    renderer_sdl_execute(DEBUG_sdl_renderer, dbg_Renderer);
    SDL_UpdateWindowSurface(dbg_Window);
}

void DEBUG_RENDER_DEBUG_INFORMATION()
{
    Ball *ball_a = &DEBUG_game_state->balls[DEBUG_DIRECTION_BALL_A_ID];
    Ball *ball_b = &DEBUG_game_state->balls[DEBUG_DIRECTION_BALL_B_ID];
    F32 length = 50.0f;
    Vec2Dim<F32> dir_vecotr_a = DEBUG_DIRECTION_BALL_A * length;
    Vec2Dim<F32> dir_vecotr_b = DEBUG_DIRECTION_BALL_B * length;
    
    RGBA_U8 direction_color = {0x00, 0x00, 0xff, 0xff};
    renderer_push_command(dbg_Renderer, 
                          RENDERER_COMMAND_SET_RENDER_COLOR, 
                          &direction_color);
    renderer_push_command(dbg_Renderer, RENDERER_COMMAND_DRAW_LINE,
                          (S32)ball_a->pos.x,
                          (S32)ball_a->pos.y,
                          (S32)ball_a->pos.x + ((S32)dir_vecotr_a.x),
                          (S32)ball_a->pos.y + ((S32)dir_vecotr_a.y));
    renderer_push_command(dbg_Renderer, RENDERER_COMMAND_DRAW_LINE,
                          (S32)ball_b->pos.x,
                          (S32)ball_b->pos.y,
                          (S32)ball_b->pos.x + ((S32)dir_vecotr_b.x),
                          (S32)ball_b->pos.y + ((S32)dir_vecotr_b.y));
    
    renderer_sdl_execute(DEBUG_sdl_renderer, dbg_Renderer);
    SDL_UpdateWindowSurface(dbg_Window);
}
*/
