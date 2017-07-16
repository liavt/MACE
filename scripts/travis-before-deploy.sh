#!/bin/bash

cd build
echo "Generating static build release..."
cmake -DMACE_TESTS=OFF -DMACE_DEMOS=OFF -DCMAKE_BUILD_TYPE=Release -DMACE_ALL_WARNINGS=OFF -DBUILD_SHARED_LIBS=OFF ..
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

git config --global user.email "travis@travis-ci.org"
git config --global user.name "Travis CI"

git remote add origin-pages https://${GITHUB_TOKEN}@github.com/liavt/MACE.git > /dev/null 2>&1

if[[ $COMPILER == g++-6 ]]; then
	git tag -d unstable;
	git push --quiet --set-upstream origin-pages :refs/tags/unstable;
	git tag -a unstable -m "This is the absolute latest version of MACE, with no guarentee it is stable.";
	git push --quiet origin-pages --tags;
else
	git tag unstable
	git push --quiet origin-pages --tags;
fi