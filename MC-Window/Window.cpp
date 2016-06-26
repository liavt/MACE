#include <MC-Window/Window.h>

namespace mc {
	Window::Window(int width, int height, const char* title) {
		m_window = SDL_CreateWindow(title,
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			width, height, SDL_WINDOW_SHOWN);

		SDL_SetWindowData(m_window, "open", (void*)(1));
	}

	bool Window::isOpen() {
		return ((int) SDL_GetWindowData(m_window, "open")) != 0;
	}

	SDL_Window* Window::getSDLWindow() {
		return m_window;
	}

	void Window::destroy() {
		SDL_DestroyWindow(m_window);
	}

}