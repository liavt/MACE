#include <MC-Graphics/GL/RendererGL.h>

namespace mc {
	RendererGL::RendererGL() {

	}

	void RendererGL::initFlags() {
		flags = SDL_WINDOW_OPENGL;
	}

	void RendererGL::init(Window* window) {
		SDL_Window* win = window->getSDLWindow();

		m_context = SDL_GL_CreateContext(win);
	}

	void RendererGL::swapBuffers(Window* window) {
		SDL_GL_SwapWindow(window->getSDLWindow());
	}

	void RendererGL::destroy() {
		SDL_GL_DeleteContext(m_context);
	}

	void RendererGL::render() {

	}
}