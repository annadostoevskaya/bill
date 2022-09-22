@echo off

set DEV_OPTS=-D_ENABLED_ASSERT -D_DEVELOPER_MODE
set CL_OPTS=-FC -GR- -EHa- -nologo -Zi -WX -W4 %DEV_OPTS%
set SRC=%cd%

set SDL_DIR=%SRC%/thirdparty/SDL2
set SDL_LIB_PATH=%SDL_DIR%/lib
set SDL_INC_PATH=%SDL_DIR%/include

pushd build

@REM TODO(annad): /SUBSYSTEM:CONSOLE, we must switch to :WINDOWS for release!

cl %CL_OPTS% %SRC%\sdl_bill.cpp -Fesdl_bill.exe -I%SDL_INC_PATH%^
  /link /LIBPATH:%SDL_LIB_PATH% SDL2main.lib SDL2.lib shell32.lib /SUBSYSTEM:CONSOLE
popd
