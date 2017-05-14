#!/bin/sh

sudo apt-get update
sudo apt-get -y install cmake libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
make
./game