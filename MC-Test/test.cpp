#include <MC-System/System.h>
#include <iostream>
#include <SDL/SDL.h>
#include <MC-Window/Window.h>

using namespace mc;

int main(int argc, char** argv) {
	System::init();

	win::Window window(800, 600, "Hello!");
	window.create();

	while (window.isOpen()) {
		System::update();
	}

	window.destroy();

	System::terminate();

	return 0;
}
