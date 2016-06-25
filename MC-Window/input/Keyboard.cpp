#include <MC-Window/input/Keyboard.h>

namespace mc {
	bool Keyboard::m_keys[GLFW_KEY_LAST];
	bool Keyboard::m_lastKeys[GLFW_KEY_LAST];

	void Keyboard::init() {
		for (int i = 0; i < GLFW_KEY_LAST;i++) {
			m_keys[i] = false;
			m_lastKeys[i] = false;
		}
	}

	void Keyboard::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (action == GLFW_PRESS) {
			m_keys[key] = true;
		} else if (action == GLFW_RELEASE) {
			m_keys[key] = false;
		}
	}

	bool Keyboard::isKeyPressed(int key) {
		bool result = m_keys[key] && !m_lastKeys[key];
		m_lastKeys[key] = m_keys[key];
		return result;
	}

	bool Keyboard::isKeyDown(int key) {
		return m_keys[key];
	}
}