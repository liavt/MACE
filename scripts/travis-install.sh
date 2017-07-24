#!/bin/bash

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
	sudo bash ./scripts/install-brew.sh;
fi

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
	sudo bash ./scripts/install-apt.sh;
fi