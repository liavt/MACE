#include <MC-Window/Window.h>

using namespace mc;

int main() {
	System::init();

	Window window(800, 600, "MACE Window test!", false);

	while (!window.isCloseRequested()) {
		if (Keyboard::isKeyPressed(GLFW_KEY_SPACE)) {
			std::cout << "Space pressed!\n";
		}

		if (Keyboard::isKeyDown(GLFW_KEY_SPACE)) {
			std::cout << "Space down!\n";
		}

		System::pollevents();
	}

	window.destroy();

	System::terminate();

	return 0;
}