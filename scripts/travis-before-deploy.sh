#!/bin/bash

cd build
echo "Generating x32 release..."
cmake -DMACE_TESTS=OFF -DMACE_DEMOS=OFF -DCMAKE_BUILD_TYPE=Release -DMACE_ALL_WARNINGS=OFF ..
make MACE -j4
ls
cd ..
mkdir release
mkdir release/static
cp build/libMACE.a release/static/libMACE.a
cp -a build/lib/. release/static/lib/
cp -a build/include/. release/static/include/
cp -a include/. release/static/include/
rm -rf build
mkdir build
cd build
echo "Generating x64 release..."
cmake -DMACE_TESTS=OFF -DMACE_DEMOS=OFF -DCMAKE_BUILD_TYPE=Release -DMACE_ALL_WARNINGS=OFF -DBUILD_SHARED_LIBS=ON ..
make MACE -j4
ls
cd ..
mkdir release/shared
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then cp build/libMACE.dylib release/shared/libMACE.dylib; fi
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then cp build/libMACE.so release/shared/libMACE.so; fi
cp -a build/lib/. release/shared/lib/
cp -a build/include/. release/shared/include/
cp -a include/. release/shared/include/
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then tar -zcf osx-x64.tar.gz release; fi
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then tar -zcf linux-x64.tar.gz release; fi