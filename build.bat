@ECHO OFF

WHERE /q cl
IF NOT %ERRORLEVEL% == 0 (
   CALL "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
)

SET CPP=clang++
SET SRC=%cd%
SET SDL_OPTS=-I %SRC%\3rdparty\SDL2\include\ -L %SRC%\3rdparty\SDL2\lib\ -lSDL2 -lSDL2main -lShell32
SET DEV_OPTS=-D _ENABLED_ASSERT -D _CLI_DEV_MODE
SET OPTIONS=-O1 -std=c++11 -msse4.1 -g %DEV_OPTS% %SDL_OPTS%

IF NOT EXIST build MKDIR build
PUSHD build
@REM TODO(annad): /SUBSYSTEM:CONSOLE, we must switch to :WINDOWS for release!
%SRC%/dev/ctime/ctime.exe -begin bill.ctm
%CPP% %OPTIONS% %SRC%\sdl_bill.cpp %OPTIONS% -o sdl_bill.exe^
    -Xlinker /subsystem:console
SET LastError=%ERRORLEVEL%
%SRC%/dev/ctime/ctime.exe -end bill.ctm %LastError%

IF NOT EXIST SDL2.dll IF EXIST ..\3rdparty\SDL2\bin\SDL2.dll (
    COPY ..\3rdparty\SDL2\bin\SDL2.dll SDL2.dll
)

POPD

IF NOT "%USERNAME%" == "annad" PAUSE

EXIT /B %LastError%
