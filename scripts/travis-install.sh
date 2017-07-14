#!/bin/bash

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
	sudo ./scripts/install-brew.sh;
fi

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
	sudo ./scripts/install-apt.sh;
fi