@ECHO OFF

SET CL_OPTS=-FC -GR- -EHa- -nologo -Zi -WX -W4 
SET SRC=%cd%

cl %CL_OPTS% %SRC%\main.cpp -Femain.exe /link /SUBSYSTEM:CONSOLE
SET LastError=%ERRORLEVEL%


EXIT /B %LastError%
