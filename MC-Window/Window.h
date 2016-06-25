#pragma once

#include <GLFW/glfw3.h>
#include <MC-System/System.h>

namespace mc {
	class Window {
	public:
		Window(int width, int height, const char* title, bool resizeable);

		int getWidth() { return m_currentWidth; };
		int getHeight() { return m_currentWidth; };
		int getOriginalWidth() { return m_originalWidth; };
		int getOriginalHeight() { return m_originalWidth; };
		bool isCloseRequested();
		GLFWwindow* getGLFWWindow();
		void destroy();
	private:
		GLFWwindow* m_window;
		int m_originalWidth;
		int m_originalHeight;
		int m_currentWidth;
		int m_currentHeight;
	};
}