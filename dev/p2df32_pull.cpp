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

inline void P2DF32_pull_reset(P2DF32_pull *pull)
{
    pull->cursor = 0;
}

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

