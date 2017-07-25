#!/bin/bash
add-apt-repository ppa:glasen/freetype2
apt-get install libopencv-dev libopenal-dev libfreetype6 libfreetype6-dev libglew-dev
git clone https://github.com/glfw/glfw.git ../build/glfw 
cd ../build/glfw
git checkout latest
cmake . -DBUILD_SHARED_LIBS=ON 
make
make install
ldconfig
cd ../..
