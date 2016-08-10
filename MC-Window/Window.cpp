/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
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
			SDL_Init(SDL_INIT_VIDEO);

			m_window = SDL_CreateWindow(m_title,
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				m_originalWidth, m_originalHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

			SDL_SetWindowData(m_window, "open", (void*)(1));
		}

		bool Window::poll()
		{
			SDL_Event e;
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
					SDL_SetWindowData(SDL_GetWindowFromID(e.window.windowID), "open", 0);
					return true;
				}
			}
			return false;
		}

		bool Window::isOpen() {
			return ((long)SDL_GetWindowData(m_window, "open")) != 0;
		}

		void poll() {
			
		}

		SDL_Window* Window::getSDLWindow() {
			return m_window;
		}

		const unsigned int & Window::getFPS() const
		{
			return fps;
		}

		void Window::setFPS(const unsigned int & FPS)
		{
			fps = FPS;
		}

		void Window::destroy() {
			SDL_DestroyWindow(m_window);
		}
	}
}