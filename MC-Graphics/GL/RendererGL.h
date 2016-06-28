#pragma once

#include <MC-Graphics/Renderer.h>
#include <SDL/SDL_opengl.h>

namespace mc {
	class RendererGL : public Renderer {
	private:
		SDL_GLContext m_context;
	public:
		RendererGL();
		void init(Window* window);
		void swapBuffers(Window* window);
		void destroy();
		void initFlags();
		void clear(Color* color);
		virtual void render(GraphicsObject* object);
	};
}
