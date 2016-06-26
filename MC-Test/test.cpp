#include <MC-System/System.h>
#include <MC-Window/Window.h>
#include <MC-Graphics/GL/RendererGL11.h>
#include <iostream>

using namespace mc;

int main(int argc, char** argv) {
	System::init();

	Window window(800, 600, "MACE SDL Window!");
	Renderer* renderer = new RendererGL11(&window);
	renderer->init();

	while (window.isOpen()) {
		System::pollevents();
		renderer->render();
		renderer->swapBuffers();
	}

	renderer->destroy();
	System::terminate();

	return 0;
}