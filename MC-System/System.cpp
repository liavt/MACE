#include <MC-System/System.h>

typedef unsigned char byte;

namespace mc {
	void System::init() {
		glfwInit();
	}

	void System::terminate() {
		glfwTerminate();
	}

	void System::pollevents() {
		glfwPollEvents();
	}
}