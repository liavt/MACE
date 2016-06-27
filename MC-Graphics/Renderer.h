#pragma once

#include <MC-Window/Window.h>
#include <iostream>

namespace mc {
	class Renderer {
	public:
		Renderer::Renderer();
		virtual void init(Window* window);
		virtual void initFlags();
		virtual void swapBuffers(Window* window);
		virtual void destroy();
		virtual void render();

		SDL_WindowFlags flags;
	};
}
