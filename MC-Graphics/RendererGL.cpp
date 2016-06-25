#include <MC-Graphics/RendererGL.h>

namespace mc {
	RendererGL::RendererGL(Window* window) : Renderer(window) {
		
	}

	void RendererGL::init() {
		glfwMakeContextCurrent(m_window->getGLFWWindow());
	}

	void RendererGL::swapBuffers() {
		glfwSwapBuffers(m_window->getGLFWWindow());
	}
}