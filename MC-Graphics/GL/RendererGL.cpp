#include <MC-Graphics/GL/RendererGL.h>

namespace mc {
	RendererGL::RendererGL(Window* window) : Renderer(window) {

	}

	void RendererGL::init() {
		SDL_Window* win = m_window->getSDLWindow();

		m_context = SDL_GL_CreateContext(win);
	}

	void RendererGL::swapBuffers() {
		SDL_GL_SwapWindow(m_window->getSDLWindow());
	}

	void RendererGL::destroy() {
		SDL_GL_DeleteContext(m_context);
	}

	void RendererGL::render() {
		
	}
}