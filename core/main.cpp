/* 
Author: github.com/annadostoevskaya
File: main.cpp
Date: December 29th 2022 11:30 pm 

Description: <empty>
*/

#include "base_types.h"
#include "memory.h"
#include "memory.cpp"
#include "memory_malloc.cpp"


int main()
{
    M_BaseMemory *base = m_malloc_base_memory();
    M_Arena arena = m_make_arena(base);
    void *some_important_obj = m_arena_push(&arena, 16);
    char *string_with_length_16 = (char*)some_important_obj;
    for(S32 i = 0; i < 16; i += 1)
    {
        string_with_length_16[i] = (char)('a' + i);
    }
    
    printf("%s\n", string_with_length_16);
    m_arena_pop_to(&arena, 0);
    // m_make_arena_reserve(base, 1024);
    // __debugbreak();
    return 0;
}

