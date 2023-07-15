#!/bin/bash

# Navigate to parent directory and create a 'build' directory there
cd ..
mkdir -p build
pushd build

# Compile the source files, generate OBJ but don't link yet
gcc -I /usr/local/include/SDL2/ -c ../code/main.cpp

# Link the object file to the libraries for final executable
gcc main.o -lSDL2 -lSDL2main -lstdc++ -o main

# Navigate back to the original directory
popd
