#include <MC-Window/Window.h>

namespace mc {
	namespace win
	{
		Window::Window(int width, int height, const char* title) {
			m_originalWidth = width;
			m_originalHeight = height;
			m_title = (char*)title;
		}

		void Window::create() {
			m_window = SDL_CreateWindow(m_title,
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				m_originalWidth, m_originalHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

			SDL_SetWindowData(m_window, "open", (void*)(1));
		}

		bool Window::isOpen() {
			return ((int)SDL_GetWindowData(m_window, "open")) != 0;
		}

		SDL_Window* Window::getSDLWindow() {
			return m_window;
		}

		void Window::destroy() {
			SDL_DestroyWindow(m_window);
		}
	}
}