/* 
Author: github.com/annadostoevskaya
File: memory_arena.cpp
Date: September 24th 2022 8:07 pm 

Description: <empty>
*/

// TODO(annad): Solve "malloc vs VirtualAlloc" problem.
// #include <stdlib.h>

typedef struct MemArena
{
    void *base;
    U64 size;
    U64 pos;
} MemArena;

/* 
// NOTE(annad): We allocated memory on platform layer and just pass pointer to this lib.
MemArena MemArena_stdlib_make_mem_arena(U64 mem_size)
{
    MemArena result;
    
    result.base = malloc(mem_size);
    result.size = mem_size;
    result.pos = 0;
    
    MemoryZero(result.base, result.size);
    
    return result;
}

void MemArena_stdlib_free_mem_arena(MemArena *mem_arena)
{
    free(mem_arena->base);
}
 */

void *MemArena_push(MemArena* mem_arena, U64 mem_size)
{
    void *result = 0;
    if(mem_arena->pos + mem_size < mem_arena->size)
    {
        result = (void*)(((U8*)mem_arena->base) + mem_arena->pos);
        mem_arena->pos += mem_size;
    }
    
    return result;
}

void MemArena_pop(MemArena *mem_arena, U64 pos)
{
    if(mem_arena->pos > pos && pos >= 0)
    {
        mem_arena->pos = pos;
    }
}

#define MemArena_PushStruct(mem_arena, T)       MemArena_push(mem_arena, sizeof(T))
#define MemArena_PushArray(mem_arena, T, count) MemArena_push(mem_arena, sizeof(T) * count)

