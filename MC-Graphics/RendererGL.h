#pragma once

#include <MC-Graphics/Renderer.h>
#include <GLFW/glfw3.h>

namespace mc {
	class RendererGL : public Renderer {
	public:
		RendererGL(Window* window);
		void init();
		void swapBuffers();
	};
}