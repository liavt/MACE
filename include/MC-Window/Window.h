#pragma once

#include <iostream>
#include <SDL/SDL.h>

namespace mc {
	/**
	Namespace with classes relating to input and windows.
	*/
	namespace win
	{
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
}
