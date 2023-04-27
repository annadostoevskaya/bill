/*
Author: github.com/annadostoevskaya
File: p2df32_pull.cpp
Date: 05/03/23 13:08:15

Description: <empty>
*/

struct P2DF32_pull
{
    P2DF32 buffer[3];
    U32 cursor;
};

inline U32 P2DF32_pull_size(P2DF32_pull *pull)
{
    return sizeof(pull->buffer) / sizeof(pull->buffer[0]);
}

void P2DF32_pull_push(P2DF32_pull *pull, P2DF32 point)
{
    if (pull->cursor >= P2DF32_pull_size(pull)) {
        return;
    }

    pull->buffer[pull->cursor] = point;
    pull->cursor += 1;
}

void P2DF32_pull_print(P2DF32_pull *pull)
{
    for (U32 i = 0; i < pull->cursor; i += 1)
    {
        printf("pull[%d]: x=%f, y=%f\n", i, 
            pull->buffer[i].x, pull->buffer[i].y);
    }
}

#if 0
void P2DF32_pull_draw(RendererHandle *hRenderer, Entity *e, F32 radius, P2DF32_pull *pull)
{
    for (U32 i = 1; i < P2DF32_pull_size(pull); i += 1)
    {
        P2DF32 a = pull->buffer[i - 1];
        P2DF32 b = pull->buffer[i];
        B8 isCollide = ballCheckWallCollide(e, radius, a, b, 1.0f);
        if (isCollide)
        {
            Renderer_pushCmd(hRenderer, 
                    RCMD_SET_RENDER_COLOR, 
                    0xff, 0x00, 0x00, 0xff);
        }

        Renderer_pushCmd(hRenderer, RCMD_DRAW_LINE, 
            (S32)a.x, (S32)a.y, 
            (S32)b.x, (S32)b.y);

        if (isCollide)
        {
            Renderer_pushCmd(hRenderer, 
                    RCMD_SET_RENDER_COLOR, 
                    0xff, 0xff, 0xff, 0xff);
        }
    }

    if (P2DF32_pull_size(pull) > 2)
    {
        Renderer_pushCmd(hRenderer, RCMD_DRAW_LINE,
            (S32)pull->buffer[pull->cursor - 1].x, (S32)pull->buffer[pull->cursor - 1].y,
            (S32)pull->buffer[0].x, (S32)pull->buffer[0].y);
    }
}
#endif

void P2DF32_pull_draw_bezier_curves(Screen *s, P2DF32_pull *pull)
{
    
}
