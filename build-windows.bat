@echo off
REM Build script for Windows (MinGW). Run from the project root (D:\CG\CG-TP1) in PowerShell or cmd.exe.
cd src
echo Cleaning object files...
del *.o 2>nul
echo Compiling all sources in src\ to ..\jogo_educativo.exe
REM Link against freeglut. If you only have glut32, change -lfreeglut to -lglut32.
g++ -std=gnu++11 -O2 -Wall -o ..\jogo_educativo.exe *.cpp -lfreeglut -lopengl32 -lglu32
if %ERRORLEVEL% NEQ 0 (
    echo Build failed. If you get "cannot find -lfreeglut", either install freeglut or change -lfreeglut to -lglut32 in this script.
    pause
) else (
    echo Build succeeded: ..\jogo_educativo.exe
)
