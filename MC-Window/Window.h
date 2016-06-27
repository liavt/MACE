#pragma once

#include <SDL/SDL.h>

namespace mc {
	class Window {
	protected:
		SDL_Window* m_window;
		int m_originalWidth;
		int m_originalHeight;
		char* m_title;
	public:
		Window(int width, int height, const char* title);
		bool isOpen();
		virtual void create();
		SDL_Window* getSDLWindow();
		void destroy();
	};
}