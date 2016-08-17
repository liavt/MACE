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
			m_window = glfwCreateWindow(m_originalWidth,m_originalHeight,m_title,NULL,NULL);
		}

		bool Window::poll()
		{
			glfwPollEvents();
			return glfwWindowShouldClose(m_window);
		}

		GLFWwindow* Window::getGLFWWindow() {
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
			glfwDestroyWindow(m_window);
		}
	}
}