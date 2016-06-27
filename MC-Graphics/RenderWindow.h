#pragma once

#include <MC-Window/Window.h>
#include <MC-Graphics/Renderer.h>

namespace mc {
	class RenderWindow : public Window{
	private:
		Renderer* m_renderer;
	public:
		RenderWindow(int width, int height, const char* title, Renderer* renderer);
		void create();
	};
}