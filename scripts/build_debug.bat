@echo off
title qmake and nmake build prompt
set MINGW_PATH=d:\Qt\Tools\mingw730_32
set QT_DIR=d:\Qt\5.12.11\mingw73_32
set BUILD_DIR=%cd%\build
set PRO_DIR=%cd%
set PATH=%MINGW_PATH%\bin;%QT_DIR%\bin;%PATH%


if not exist %BUILD_DIR% (
    md %BUILD_DIR%
)

cd build
qmake.exe %PRO_DIR%\test.pro -spec win32-g++ "CONFIG+=debug" "CONFIG+=console"
if exist %BUILD_DIR%\debug\test.exe del %BUILD_DIR%\debug\test.exe
@REM d:\Qt\Tools\QtCreator\bin\jom.exe -j4
%MINGW_PATH%\bin\mingw32-make -f Makefile.Debug
cd debug
if not exist %BUILD_DIR%\debug\Qt5Core.dll (
    windeployqt test.exe
)