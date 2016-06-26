#include <MC-System/System.h>
#include <MC-Window/Window.h>
#include <iostream>

int main(int argc, char** argv) {
	mc::System::init();

	mc::Window window(800, 600, "MACE SDL Window!");
	
	while (window.isOpen()) {
		mc::System::pollevents();
	}

	mc::System::terminate();

	return 0;
}