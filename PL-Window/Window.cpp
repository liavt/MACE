#include <PL-Window/Window.h>

namespace mc {
	void Window::test() {
		Engine engine;
		engine.init();

		GLFWwindow* window = glfwCreateWindow(600, 800, "Window!", NULL, NULL);
		if (window != NULL) {
			std::cout << "Created window!\n";
		}
		else {
			std::cout << "Couldn't create window!\n";
		}
		glfwDestroyWindow(window);
		engine.terminate();
	}
}