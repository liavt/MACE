

#include "Engine.h"

typedef unsigned char byte;

namespace mc {
	void Engine::init() {
		if (glfwInit()) {
			std::cout << "GLFW Inited!\n";
		}
		else {
			std::cout << "GLFW Failed!\n";
		}
	}

	void Engine::terminate() {
		glfwTerminate();
	}
}