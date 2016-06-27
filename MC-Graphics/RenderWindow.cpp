#include <MC-Graphics/RenderWindow.h>

namespace mc {
	RenderWindow::RenderWindow(int width, int height, const char* title, Renderer* renderer) : Window(width, height, title) {
		m_renderer = renderer;
	}

	void RenderWindow::create() {
		m_renderer->initFlags();

		m_window = SDL_CreateWindow(m_title,
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			m_originalWidth, m_originalHeight, SDL_WINDOW_SHOWN | m_renderer->flags);

		SDL_SetWindowData(m_window, "open", (void*)(1));

		m_renderer->init(this);
	}
}