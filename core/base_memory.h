/* 
Author: github.com/annadostoevskaya
File: base_memory.h
Date: December 29th 2022 11:02 pm 

Description: 
I wrote this file with Alan Webster 
from a series of his YouTube video tutorials.
See at this channel: https://www.youtube.com/c/Mr4thProgramming
*/

#ifndef BASE_MEMORY_H
#define BASE_MEMORY_H

////////////////////////////////
// NOTE(annad): V-Table
typedef void *M_ReserveFunc(void *ctx, U64 size);
typedef void  M_ChangeMemoryFunc(void *ctx, void *ptr, U64 size);
// typedef void  M_CommitFunc(void *ctx, void *ptr, U64 size)
// typedef void  M_DecommitFunc(void *ctx, void *ptr, U64 size)
// typedef void  M_ReleaseFunc(void *ctx, void *ptr, U64 size)

struct M_BaseMemory
{
    M_ReserveFunc      *reserve;
    M_ChangeMemoryFunc *commit;
    M_ChangeMemoryFunc *decommit;
    M_ChangeMemoryFunc *release;
    void *ctx;
};

internal void m_change_memory_noop(void *ctx, void *ptr, U64 size) 
{
    // ...
};

////////////////////////////////
// NOTE(annad): Arena 
struct M_Arena
{
    M_BaseMemory *base;
    U8  *memory;
    U64  cap;
    U64  pos;
    U64  commit_pos;
};

#define M_DEFAULT_RESERVE_SIZE GB(1)
#define M_COMMIT_BLOCK_SIZE    MB(64)

internal M_Arena m_make_arena_reserve(M_BaseMemory *base, U64 reserve);
internal M_Arena m_make_arena(M_BaseMemory *base);
internal void    m_arena_release(M_Arena *arena);
internal void*   m_arena_push(M_Arena *arena, U64 size);
internal void    m_arena_pop_to(M_Arena *arena, U64 pos);

#endif //BASE_MEMORY_H

