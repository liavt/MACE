#!/bin/bash

cd build;
make all -j4;
ls;
./MACE-Testing;
cd ..

if [[ $TRAVIS_OS_NAME == osx || $COMPILER == g++-6 ]]; then
	git config --global user.email "travis@travis-ci.org";
	git config --global user.name "Travis CI";
	
	git fetch --tags;
	TAG_NAME=$(<VERSION);
	git tag ${TAG_NAME};
	git push https://${GITHUB_TOKEN}@github.com/liavt/MACE.git --tags;
fi