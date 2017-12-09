#!/bin/sh
brew install cmake sdl2 sdl2_image sdl2_mixer sdl2_ttf
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
make
./game
