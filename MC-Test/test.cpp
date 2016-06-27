#include <MC-System/System.h>
#include <MC-Graphics/RenderWindow.h>
#include <MC-Graphics/GL/RendererGL11.h>
#include <iostream>

using namespace mc;

int main(int argc, char** argv) {
	System::init();

	Renderer* renderer = new RendererGL11();
	RenderWindow* window = new RenderWindow(800, 600, "MACE SDL Window!", renderer);
	window->create();

	while (window->isOpen()) {
		System::pollevents();
		renderer->render();
		renderer->swapBuffers(window);
	}

	renderer->destroy();
	System::terminate();

	return 0;
}