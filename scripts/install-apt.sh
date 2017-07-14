#!/bin/bash
apt-get install libopencv-dev xorg-dev libopenal-dev libfreetype6 libfreetype6-dev libglu1-mesa-dev libglew-dev libglfw2
apt-get update
git clone https://github.com/glfw/glfw.git ../build/glfw 
cd ../build/glfw
git checkout latest
cmake . -DBUILD_SHARED_LIBS=ON 
make
make install
ldconfig
cd ../..