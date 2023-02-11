/* 
Author: github.com/annadostoevskaya
File: bill_debug.cpp
Date: December 16th 2022 11:47 pm 

Description: <empty>
*/

void dbg_VerifyCollides()
{
    Entity *balls = (Entity*)&dbg_GameState->balls;
    Rect *tbl= &dbg_GameState->table;
    V2DF32 nvecwall = {};
    for (S32 i = 0; i < BALL_COUNT; i += 1)
    {
        Entity *e = &balls[i];
        if (!e->isInit) continue;
        // Walls
        Assert(ballCheckTableBoardCollide(e, tbl, &nvecwall) != true);
        // Other balls
        for (S32 j = 0; j < BALL_COUNT; j += 1)
        {
            if (i == j) continue;
            Entity *ball = &balls[j];
            if (!ball->isInit) continue;
            if (ballCheckBallCollide(e, ball))
            {
                dbg_ForceUpdateScreen();
                DbgPrint("[VerifyCollides] d = %f", (e->p - ball->p).getLength());
                Assert(false);
            }
        }
    }
}

void dbg_ForceUpdateScreen()
{
    Renderer_pushCmd(dbg_HRenderer, RCMD_SET_RENDER_COLOR, 0xff, 0xff, 0xff, 0xff);
    Entity *balls = (Entity *)&dbg_GameState->balls;
    for (S32 i = 0; i < BALL_COUNT; i += 1)
    {
        Entity *e = &balls[i];
        if (e->isInit)
        {
            Renderer_pushCmd(dbg_HRenderer, RCMD_DRAW_CIRCLE, (S32)e->p.x, (S32)e->p.y, (S32)BALL_RADIUS);
        }
    }

    Renderer_pushCmd(dbg_HRenderer, RCMD_NULL);
    SDLRenderer_exec(dbg_HRenderer);
    SDL_RenderPresent(dbg_SdlRenderer);
    SDL_UpdateWindowSurface(dbg_Window);
}

