#!/bin/bash

make all -j4;
ls;
./MACE-Testing;
cd ..;

if [[ $TRAVIS_OS_NAME == osx || $COMPILER == g++-6 ]]; then
	git config --global user.email "travis@travis-ci.org";
	git config --global user.name "Travis CI";

	TAG_NAME=$(<VERSION);
	if [[ $COMPILER == g++-6 ]]; then
		git tag -a ${TAG_NAME} -m "Version $TAG_NAME";
	else
		git tag ${TAG_NAME};
	fi
	git push https://${GITHUB_TOKEN}@github.com/liavt/MACE.git --tags;
fi