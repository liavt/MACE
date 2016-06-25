#pragma once

#include <GLFW/glfw3.h>

namespace mc {
	class Keyboard {
	private:
		static bool m_keys[GLFW_KEY_LAST];
		static bool m_lastKeys[GLFW_KEY_LAST];
	public:
		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void init();
		static bool isKeyPressed(int key);
		static bool isKeyDown(int key);
	};
}