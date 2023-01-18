/* 
Author: github.com/annadostoevskaya
File: memory.cpp
Date: January 1st 2023 4:18 am 

Description: 
I wrote this file with Alan Webster 
from a series of his YouTube video tutorials.
See at this channel: https://www.youtube.com/c/Mr4thProgramming
*/


////////////////////////////////
// NOTE(annad): Arena function

#if _CLI_DEV_MODE
# pragma warning(push)
# pragma warning(disable : 4505)
#endif // _CLI_DEV_MODE

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

internal void m_destroy_arena(M_Arena *arena)
{
    arena->base->release(arena->base->ctx, 
                         (void*)arena->memory, 
                         arena->cap);
}

internal void* m_arena_push(M_Arena *arena, U64 size)
{
    void *result = 0;
    if(arena->pos + size <= arena->cap)
    {
        result = arena->memory + arena->pos;
        arena->pos += size;
        U64 p = arena->pos;
        U64 commit_p = arena->commit_pos;
        if(p > commit_p)
        {
            U64 p_aligned = AlignUpPow2(p, M_COMMIT_BLOCK_SIZE - 1);
            U64 next_commit_p = ClampTop(p_aligned, arena->cap);
            U64 commit_size = next_commit_p - commit_p;
            M_BaseMemory *base = arena->base;
            base->commit(base->ctx, arena->memory + commit_p, commit_size);
            arena->commit_pos = next_commit_p;
        }
    }
    
    return result;
}

internal void m_arena_pop_to(M_Arena *arena, U64 pos)
{
    if(pos < arena->pos)
    {
        arena->pos = pos;
        
        U64 p = arena->pos;
        U64 p_aligned = AlignUpPow2(p, M_COMMIT_BLOCK_SIZE - 1);
        U64 next_commit_p = ClampTop(p_aligned, arena->cap);
        
        U64 commit_p = arena->commit_pos;
        if(next_commit_p < commit_p)
        {
            U64 decommit_size = commit_p - next_commit_p;
            
            M_BaseMemory *base = arena->base;
            base->decommit(base->ctx, arena->memory + commit_p, decommit_size);
            arena->commit_pos = next_commit_p;
        }
    }
}

#if _CLI_DEV_MODE
# pragma warning(pop)
#endif // _CLI_DEV_MODE

