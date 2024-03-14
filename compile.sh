#!/bin/bash

rm -rf build/
mkdir -p build/
# Enter the build directory (or quit if we fail to do so)
cd build || exit
# Compile the project
cmake ..
make -j8
cd ..

# Copy the .uf2 file into the root directory
cp build/src/*.uf2 .
