/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Window/Window.h>
#include <MACE/System/System.h>
#include <cstring>

namespace mc {
	namespace win
	{
		Window::Window(const int width, const int height, const char* windowTitle) : title(windowTitle), originalWidth(width), originalHeight(height){
		}

		void Window::create() {
			window = glfwCreateWindow(originalWidth,originalHeight,title.c_str(),NULL,NULL);

			auto closeCallback = [](GLFWwindow* win) {mc::System::requestStop();};
			glfwSetWindowCloseCallback(window,closeCallback);
		}

		void Window::poll()
		{
			glfwPollEvents();
		}

		GLFWwindow* Window::getGLFWWindow() {
			return window;
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
			glfwDestroyWindow(window);
		}
		const int Window::getOriginalWidth() const
		{
			return originalWidth;
		}
		const int Window::getOriginalHeight() const
		{
			return originalHeight;
		}
		std::string Window::getTitle()
		{
			return title;
		}
		const std::string Window::getTitle() const
		{
			return title;
		}
		void Window::setTitle(const std::string & newTitle)
		{
			glfwSetWindowTitle(window, newTitle.c_str());
		}
	}
}