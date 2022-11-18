@ECHO OFF

SET DEV_OPTS=-D_ENABLED_ASSERT -D_DEVELOPER_MODE
SET CL_OPTS=-FC -GR- -EHa- -nologo -Zi -WX -W4 %DEV_OPTS%
SET SRC=%cd%

SET SDL_DIR=%SRC%/thirdparty/SDL2
SET SDL_LIB_PATH=%SDL_DIR%/lib
SET SDL_INC_PATH=%SDL_DIR%/include

IF NOT EXIST build MKDIR build
PUSHD build
@REM TODO(annad): /SUBSYSTEM:CONSOLE, we must switch to :WINDOWS for release!
%SRC%/dev/ctime/ctime.exe -begin bill.ctm
cl %CL_OPTS% %SRC%\sdl_bill.cpp -Fesdl_bill.exe -I%SDL_INC_PATH%^
  /link /LIBPATH:%SDL_LIB_PATH% SDL2main.lib SDL2.lib shell32.lib /SUBSYSTEM:CONSOLE
SET LastError=%ERRORLEVEL%
%SRC%/dev/ctime/ctime.exe -end bill.ctm %LastError%

IF NOT EXIST SDL2.dll (
    IF EXIST ..\thirdparty\SDL2\bin\SDL2.dll (
        COPY ..\thirdparty\SDL2\bin\SDL2.dll SDL2.dll
    )
)

POPD

EXIT /B %LastError%
