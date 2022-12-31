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
    __debugbreak();
    return 0;
}

