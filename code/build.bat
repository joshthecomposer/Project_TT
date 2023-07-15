@echo off

mkdir ..\build
pushd ..\build

::compile the source files, generate OBJ but don't link yet
cl ..\code\main.cpp /I ..\code\include\SDL2\ /c 

::LInk the object file to the libraries for final executable
link main.obj shell32.lib ..\data\libs\SDL2.lib ..\data\libs\SDL2main.lib ..\data\libs\SDL2test.lib /SUBSYSTEM:CONSOLE

popd