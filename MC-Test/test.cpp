#include <MC-Graphics/RendererGL.h>
#include <MC-Window/Window.h>

using namespace mc;

int main() {
	System::init();

	Window window(800, 600, "MACE Window test!", false);
	Renderer* renderer = new RendererGL(&window);
	renderer->init();

	glClearColor(1, 0, 1, 1);

	while (!window.isCloseRequested()) {
		glClear(GL_COLOR_BUFFER_BIT);

		if (Keyboard::isKeyPressed(GLFW_KEY_SPACE)) {
			std::cout << "Space pressed!\n";
		}

		if (Keyboard::isKeyDown(GLFW_KEY_SPACE)) {
			std::cout << "Space down!\n";
		}

		renderer->swapBuffers();

		System::pollevents();
	}

	window.destroy();

	System::terminate();

	return 0;
}