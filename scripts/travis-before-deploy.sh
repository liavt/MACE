#!/bin/bash

cd build
echo "Generating static build release..."
cmake -DMACE_TESTS=OFF -DMACE_DEMOS=OFF -DCMAKE_BUILD_TYPE=Release -DMACE_ALL_WARNINGS=OFF ..
make MACE -j4
ls
cd ..
mkdir release
mkdir release/static
cp build/libMACE.a release/static/libMACE.a
cp -a build/include/. release/static/include/
cp -a include/. release/static/include/
rm -rf build
mkdir build
cd build
echo "Generating shared build release..."
cmake -DMACE_TESTS=OFF -DMACE_DEMOS=OFF -DCMAKE_BUILD_TYPE=Release -DMACE_ALL_WARNINGS=OFF -DBUILD_SHARED_LIBS=ON ..
make MACE -j4
ls
cd ..
mkdir release/shared
cp -a build/include/. release/shared/include/
cp -a include/. release/shared/include/
if [[ -e build/libMACE.dylib ]]; then
	cp build/libMACE.dylib release/shared/libMACE.dylib;
	tar -zcf osx-x64.tar.gz release;
fi
if [[ -e build/libMACE.so ]]; then
	cp build/libMACE.so release/shared/libMACE.so;
	tar -zcf linux-x64.tar.gz release;
fi
ls