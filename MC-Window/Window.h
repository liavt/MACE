#pragma once

#include <SDL/SDL.h>
#include <iostream>

namespace mc {
	class Window {
		friend class WindowModule;

	protected:
		SDL_Window* m_window;
		int m_originalWidth;
		int m_originalHeight;
		char* m_title;
	public:
		Window(int width, int height, const char* title);
		bool isOpen();
		SDL_Window* getSDLWindow();

		virtual void create();
		virtual void destroy();
	};
}
