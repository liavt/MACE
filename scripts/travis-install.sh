#!/bin/bash

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
	sudo ./scripts/install-brew.sh;
fi

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
	sudo ./scripts/install-apt.sh;
	add-apt-repository -y ppa:oibaf/graphics-drivers;
	sudo apt-get install xorg-dev libgl1-mesa-dev libgl1-mesa-glx mesa-common-dev libglapi-mesa libgbm1 libgl1-mesa-dri libxatracker-dev xvfb
	apt-get update --qq  -y;
	export DEBIAN_FRONTEND=noninteractive;
	export DISPLAY=:99l
	export LIBGL_ALWAYS_SOFTWARE=1;
	xpra --xvfb="Xorg +extension GLX +extension RANDR +extension RENDER -config `pwd`/test/dummy.xorg.conf -logfile ${HOME}/.xpra/xorg.log"  start :99;
	sleep 3 ;
	LIBGL_ALWAYS_SOFTWARE=1 glxinfo;
	glxinfo;
	cat ${HOME}/.xpra/xorg.log;
fi