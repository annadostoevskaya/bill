/* 
Author: github.com/annadostoevskaya
File: types.h
Date: September 12th 2022 6:13 pm 

Description: 
I wrote this file with Alan Webster 
from a series of his YouTube video tutorials.
See at this channel: https://www.youtube.com/c/Mr4thProgramming
*/

#ifndef TYPES_H 
#define TYPES_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

////////////////////////////////
// NOTE(annad): base_context.h
// TODO(annad): Architecture context!
#if defined(_MSC_VER) && !defined(__clang__)
# define _COMPILER_CL 1
# if defined(_WIN32)
#  define _OS_WINDOWS 1
# else
#  error missing OS detection
# endif
#elif defined(__GNUC__)  && !defined(__llvm__) && !defined(__clang__)
# define _COMPILER_GCC 1
# if defined(_WIN32)
#  define _OS_WINDOWS 1
# elif defined(__gnu_linux__)
#  define _OS_LINUX 1
# else
#  error missing OS detection
# endif
#elif defined(__clang__)
# define _COMPILER_CLANG 1
# if defined(_WIN32)
#  define _OS_WINDOWS 1
# elif defined(__gnu_linux__)
#  define _OS_LINUX 1
# else
#  error missing OS detection
# endif
#else
# error no context cracking for this compiler
#endif
// NOTE(annad): Zero fill missing context macros
#if !defined(_COMPILER_CL)
# define _COMPILER_CL 0
#endif
#if !defined(_COMPILER_CLANG)
# define _COMPILER_CLANG 0
#endif
#if !defined(_COMPILER_GCC)
# define _COMPILER_GCC 0
#endif
#if !defined(_OS_WINDOWS)
# define _OS_WINDOWS 0
#endif
#if !defined(_OS_LINUX)
# define _OS_LINUX 0
#endif

////////////////////////////////
// NOTE(annad): base_types.h
typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef S8       B8;
typedef S16      B16;
typedef S32      B32;
typedef S64      B64;
typedef float    F32;
typedef float    F64;
typedef void VoidFunc(void);

////////////////////////////////
// NOTE(annad): base_consts.h
#define MIN_S8  ((S8) (0x80))
#define MIN_S16 ((S16)(0x8000))
#define MIN_S32 ((S32)(0x80000000))
#define MIN_S64 ((S64)(0x8000000000000000llu))
#define MAX_S8  ((S8) (0x7f))
#define MAX_S16 ((S16)(0x7fff))
#define MAX_S32 ((S32)(0x7fffffff))
#define MAX_S64 ((S64)(0x7fffffffffffffffllu))
#define MAX_U8  ((U8) (0xf))
#define MAX_U16 ((U16)(0xffff))
#define MAX_U32 ((U32)(0xffffffff))
#define MAX_U64 ((U64)(0xffffffffffffffffllu))
#define PI_F32  ((F32)(3.14159265359f))
#define PI_F64  ((F64)(3.14159265359f))
#define TAU_F32 ((F32)(6.28318530718f))
#define TAU_F64 ((F64)(6.28318530718f))

////////////////////////////////
// NOTE(annad): base_marco_helpers.h
#define Stmnt(S) do { S } while(0)
#if _OS_WINDOWS
# define AssertBreak() __debugbreak()
#endif
#if _OS_LINUX
# define AssertBreak() __builtin_trap()
#endif
#if !defined(AssertBreak)
# define AssertBreak() (*((int*)(0)) = 0)
#endif
#if defined(_ENABLED_ASSERT)
# define Assert(expr) Stmnt( if(!(expr)) { AssertBreak(); } )
#else
# define Assert(expr)
#endif

#define globalv static
#define localv static
#define internal static

#define c_linkage_begin extern "C" {
#define c_linkage_end }
#define c_linkage extern "C"

//
//
//

#define MemoryCopy(d, s, z)      memmove((d), (s), (z))
#define MemoryCopyStruct(d, s)   MemoryCopy((d), (s), Min(sizeof(*(d)), sizeof(*(s))))
#define MemoryCopyArray(d, s)    MemoryCopy((d), (s), Min(sizeof(d), sizeof(s)))
#define MemoryCopyTyped(d, s, c) MemoryCopy((d), (s), Min(sizeof(*(d)), sizeof(*(s))) * (c))
#define MemoryMatch(a, b, z)  (memcmp((a), (b), (z)) == 0)
#define MemoryZero(p, z)      memset((p), 0, (z))
#define MemoryZeroStruct(p)   MemoryZero((p), sizeof(*(p)))
#define MemoryZeroArray(p)    MemoryZero((p), sizeof(p))
#define MemoryZeroTyped(p, c) MemoryZero((p), sizeof(*(p)) * (c))

//
//
//

#define EvalPrint(x)      printf("%s = %d\n",      #x, (S32)(x))
#define EvalPrintLL(x)    printf("%s = %lld\n",    #x, (S64)(x))
#define EvalPrintU(x)     printf("%s = %u\n",      #x, (U32)(x))
#define EvalPrintULL(x)   printf("%s = %llu\n",    #x, (U64)(x))
#define EvalPrintF(x)     printf("%s = %f [%e]\n", #x, (F64)(x), (F64)(x))
#define EvalPrintB(x)     printf("%s = %s\n",      #x, (char*)((x) ? "true" : "false"))
#define EvalPrintS(x)     printf("%s = %s\n",      #x, (char*)(x))
#define EvalPrintAddr(x)  printf("%s = 0x%p\n",    #x, (void*)(&x))
#define EvalPrintHex(x)   printf("%s = 0x%x\n",    #x, (unsigned int)(x))
#define EvalPrintHexLL(x) printf("%s = 0x%llx\n",  #x, (unsigned long long)(x))

//
//
//

#define Min(a, b) (((a) < (b)) ? (a) : (b))
#define ClampTop(a, b) Min(a, b)
#define AlignUpPow2(x, p)   (((x) + (p) - 1) & ~((p) - 1))
#define AlignDownPow2(x, p) ( (x)            & ~((p) - 1))

//
//
//

#define BYTE(x) (x)
#define KB(x)   ((U64)(1024 * BYTE(x)))
#define MB(x)   ((U64)(1024 * KB(x)))
#define GB(x)   ((U64)(1024 * MB(x)))

//
//
//

#endif // TYPES_H


