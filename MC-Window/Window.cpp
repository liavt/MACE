#include <MC-Window/Window.h>

namespace mc {
	Window::Window(int width, int height, const char* title, bool resizeable) {
		glfwDefaultWindowHints();
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		if (resizeable) {
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		}
		else {
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		}

		m_window = glfwCreateWindow(width, height, title, NULL, NULL);

		glfwSetKeyCallback(m_window, Keyboard::keyCallback);

		glfwShowWindow(m_window);
	}

	bool Window::isCloseRequested() {
		return glfwWindowShouldClose(m_window);
	}
	
	GLFWwindow* Window::getGLFWWindow() {
		return m_window;
	}

	void Window::destroy() {
		glfwDestroyWindow(m_window);
	}
}