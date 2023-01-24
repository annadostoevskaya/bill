/* 
Author: github.com/annadostoevskaya
File: memory_void.cpp
Date: January 3rd 2023 6:01 am 

Description: <empty>
*/

struct memory_block
{
    void *mem;
    size_t sz;
};

internal void* m_void_reserve(void *ctx, U64 size)
{
    memory_block *mb = (memory_block*)ctx;
    void *result = 0;
    if(mb->sz <= size) {
        result = mb->mem;
    }
    
    return result;
}

internal M_BaseMemory *m_void_base_memory(void *mem, size_t sz)
{
    localv M_BaseMemory memory = {};
    if(memory.reserve == 0)
    {
        memory.reserve = m_void_reserve;
        memory.commit = m_change_memory_noop;
        memory.decommit = m_change_memory_noop;
        memory.release = m_change_memory_noop;
        
        localv memory_block mb;
        mb.mem = mem;
        mb.sz = sz;
        
        memory.ctx = (void*)(&mb);
    }
    
    return &memory;
}

