#pragma once

#include <MC-Graphics/Renderer.h>
#include <SDL/SDL_opengl.h>

namespace mc {
	class RendererGL : public Renderer {
	private:
		SDL_GLContext m_context;
	public:
		RendererGL(Window* window);
		void init();
		void swapBuffers();
		void destroy();
		virtual void render();
	};
}
