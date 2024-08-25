@echo off
setlocal

set cwd=%cd%
set src_dir=%cwd%\src

call vcvarsall.bat x64 > nul 2>&1
mkdir .\build\> nul 2>&1
pushd .\build\

set entry=%src_dir%\main.cpp
set src_files=%src_dir%/core/application.cpp %src_dir%/win32/win32.cpp

set third_party=User32.lib
set includes=-I%cwd% /I"%src_dir%" ^
	%entry% %src_files% %third_party%

set warnings=-W4 -WX -wd4201 -wd4127 -wd4100 -wd4514 -wd4668 -wd5045
set exe_name=automation.exe

cl -Fe: %exe_name% -std:c++20 -nologo ^
	-Oid -GR- -EHs -MT -MP -Gm- %warnings% -FC -Zi -Fm: lox.map ^
	%includes% /link -opt:ref | gnomon

if errorlevel 1 del %exe_name% && echo Build failed...

popd
endlocal
echo:
