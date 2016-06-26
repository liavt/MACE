#pragma once

#include <MC-Window/Window.h>

namespace mc {
	class Renderer {
	protected:
		Window* m_window;
	public:
		Renderer::Renderer(Window* window);
		virtual void init();
		virtual void swapBuffers();
		virtual void destroy();
		virtual void render();
	};
}
