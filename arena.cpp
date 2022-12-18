/* 
Author: github.com/annadostoevskaya
File: arena.cpp
Date: September 24th 2022 8:07 pm 

Description: <empty>
*/

// TODO(annad): Solve "malloc vs VirtualAlloc" problem.
// #include <stdlib.h>
// vtbl

struct Arena
{
    void *base;
    U64 size;
    U64 pos;
};

void *arena_push(Arena* arena, U64 size)
{
    void *result = 0;
    if(arena->pos + size < arena->size)
    {
        result = (void*)(((U8*)arena->base) + arena->pos);
        arena->pos += size;
    }
    
    return result;
}

void arena_pop(Arena *arena, U64 pos)
{
    if(arena->pos > pos && pos >= 0)
    {
        arena->pos = pos;
    }
}

#define Arena_PushStruct(arena, T)       arena_push(arena, sizeof(T))
#define Arena_PushArray(arena, T, count) arena_push(arena, sizeof(T) * count)

