#pragma once

#include <SDL/SDL.h>

namespace mc {
	class Window {
	private:
		SDL_Window* m_window;
	public:
		Window(int width, int height, const char* title);
		bool isOpen();
		SDL_Window* getSDLWindow();
		void destroy();
	};
}