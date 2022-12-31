/* 
Author: github.com/annadostoevskaya
File: memory.cpp
Date: January 1st 2023 4:18 am 

Description: <empty>
*/


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
        U64 p = arena->pos;
        U64 commit_p = arena->commit_pos;
        if(p > commit_p)
        {
            U64 p_aligned = AlignUpPow2(p, M_COMMIT_BLOCK_SIZE - 1);
            U64 next_commit_p = ClampTop(p_aligned, arena->cap);
            U64 commit_size = next_commit_p - p_aligned;
            
            M_BaseMemory *base = arena->base;
            base->commit(base->ctx, arena->memory + p_aligned, commit_size);
            
            arena->commit_pos = next_commit_p;
        }
    }
    
    return result;
}

internal void
m_arena_pop_to(M_Arena *arena, U64 pos)
{
    if(pos < arena->pos)
    {
        arena->pos = pos;
        
        U64 p = arena->pos;
        U64 p_aligned = AlignDownPow2(p, M_COMMIT_BLOCK_SIZE - 1);
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


