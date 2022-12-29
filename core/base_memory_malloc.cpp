/* 
Author: github.com/annadostoevskaya
File: base_memory_malloc.cpp
Date: December 29th 2022 11:12 pm 

Description: 
I wrote this file with Alan Webster 
from a series of his YouTube video tutorials.
See at this channel: https://www.youtube.com/c/Mr4thProgramming
*/

#include <stdlib.h>

////////////////////////////////
// NOTE(annad): Malloc 
internal void* m_malloc_reserve(void *ctx, U64 size)
{
    return malloc(size);
}

internal void m_malloc_release(void *ctx, void *ptr, U64 size)
{
    free(ptr);
}

internal M_BaseMemory *m_malloc_base_memory()
{
    localv M_BaseMemory memory = {};
    if(memory.reserve == 0)
    {
        memory.reserve = m_malloc_reserve;
        memory.commit = m_change_memory_noop;
        memory.decommit = m_change_memory_noop;
        memory.release = m_malloc_release;
    }
    
    return &memory;
}

////////////////////////////////
// NOTE(annad): Arena function
internal M_Arena m_make_arena_reserve(M_BaseMemory *base, U64 reserve_size)
{
    M_Arena result = {};
    result.base = base;
    result.memory = (U8*)(base->reserve(base->ctx, reserve_size));
    result.cap = reserve_size;
    return result;
}

internal M_Arena m_make_arena(M_BaseMemory *base)
{
    M_Arena result = m_make_arena_reserve(base, M_DEFAULT_RESERVE_SIZE);
    return result;
}

internal void* m_arena_push(M_Arena *arena, U64 size)
{
    void *result = 0;
    if(arena->pos + size <= arena->cap)
    {
        result = arena->memory + arena->pos;
        arena->pos += size;
        if(arena->pos > arena->commit_pos)
        {
            // TODO(annad): https://youtu.be/L79vSP8yV2g?t=411
            U64 next_commit_pos = (arena->pos + M_COMMIT_BLOCK_SIZE - 1);
        }
    }
    
    return result;
}

