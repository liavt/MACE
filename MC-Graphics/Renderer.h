#pragma once

#include <MC-Window/Window.h>
#include <MC-Graphics/GraphicsObject.h>
#include <MC-System/Utility/Color.h>
#include <iostream>

namespace mc {
	class Renderer {
	public:
		Renderer::Renderer();
		virtual void init(Window* window);
		virtual void initFlags();
		virtual void swapBuffers(Window* window);
		virtual void destroy();
		virtual void clear(Color* color);
		virtual void render(GraphicsObject* object);

		SDL_WindowFlags flags;
	};
}
