@ECHO OFF

:: Prepare environment
WHERE /q cl
IF NOT %ERRORLEVEL% == 0 (
   CALL "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
)

SET SRC=%cd%

IF NOT EXIST build MKDIR build

PUSHD build
%SRC%/dev/ctime/ctime.exe -begin bill.ctm

:: Compile modules
@REM TODO(annad): /SUBSYSTEM:CONSOLE, we must switch to :WINDOWS for release!
clang++ -g -Ofast -c -std=c++11 -msse4.1 %SRC%\bill_renderer_software.cpp -o bill_renderer_software.o
clang++ -g -std=c++11 -D _ENABLED_ASSERT -D _CLI_DEV_MODE -I %SRC%\3rdparty\SDL2\include\ -L %SRC%\3rdparty\SDL2\lib\ -lSDL2 -lSDL2main -lShell32 bill_renderer_software.o %SRC%\sdl_bill.cpp -o sdl_bill.exe -Xlinker /subsystem:console
SET LastError=%ERRORLEVEL%

IF NOT EXIST SDL2.dll IF EXIST ..\3rdparty\SDL2\bin\SDL2.dll (
    COPY ..\3rdparty\SDL2\bin\SDL2.dll SDL2.dll
)

%SRC%/dev/ctime/ctime.exe -end bill.ctm %LastError%
POPD

IF NOT "%USERNAME%" == "annad" PAUSE

EXIT /B %LastError%
