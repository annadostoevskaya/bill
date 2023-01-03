/* 
Author: github.com/annadostoevskaya
File: memory_malloc.cpp
Date: December 29th 2022 11:12 pm 

Description: 
I wrote this file with Alan Webster 
from a series of his YouTube video tutorials.
See at this channel: https://www.youtube.com/c/Mr4thProgramming
*/

////////////////////////////////
// NOTE(annad): Malloc 
#if _DEVELOPER_MODE
# pragma warning(push)
# pragma warning(disable : 4100)
# pragma warning(disable : 4505)
#endif // _DEVELOPER_MODE

internal void* m_malloc_reserve(void *ctx, U64 size)
{
    (void)size; // NOTE(annad): Shut up warning C4100
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

#if _DEVELOPER_MODE
# pragma warning(pop)
#endif // _DEVELOPER_MODE
