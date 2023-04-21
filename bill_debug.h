/*
Author: github.com/annadostoevskaya
File: bill_debug.h
Date: 11/02/23 11:46:45

Description: <empty>
*/

globalv GameIO          *dbg_GameIO;
globalv GameState       *dbg_GameState;
globalv SDL_Window      *dbg_Window;
globalv SDL_Renderer    *dbg_SdlRenderer;
globalv S32             dbg_GlobalFrameCounter;

#if VA_OPT_SUPPORTED
# define DbgPrint(STR, ...) printf("[dbg] (%d) " STR "\n", dbg_GlobalFrameCounter __VA_OPT__(,) __VA_ARGS__)
#else
# define DbgPrint(STR, ...) printf("[dbg] (%d) " STR "\n", dbg_GlobalFrameCounter, __VA_ARGS__)
#endif

