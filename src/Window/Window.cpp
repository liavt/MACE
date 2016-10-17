/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

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
			setCloseCallback(closeCallback);
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
		int Window::getAttribute(const int attribute) const
		{
			return glfwGetWindowAttrib(window,attribute);
		}
		void * Window::getUserPointer() const
		{
			return glfwGetWindowUserPointer(window);
		}
		void Window::setUserPointer(void * pointer)
		{
			glfwSetWindowUserPointer(window,pointer);
		}
		int Window::shouldClose() const
		{
			return glfwWindowShouldClose(window);
		}
		void Window::setShouldClose(const int value)
		{
			glfwSetWindowShouldClose(window,value);
		}
		void Window::setCloseCallback(const GLFWwindowclosefun func)
		{
			glfwSetWindowCloseCallback(window, func);
		}
		mc::Vector2i Window::getSize() const
		{
			int width, height;
			glfwGetWindowSize(window,&width,&height);
			return {width,height};
		}
		mc::Vector4i Window::getFrameSize() const
		{
			int left, top, right, bottom;
			glfwGetWindowFrameSize(window, &left, &top, &right, &bottom);

			return{ left,top,right,bottom };
		}
		void Window::setSize(const Size width, const Size height)
		{
			glfwSetWindowSize(window, width, height);
		}
		void Window::setResizeCallback(const GLFWwindowsizefun func)
		{
			glfwSetWindowSizeCallback(window, func);
		}
		mc::Vector2i Window::getFramebufferSize() const
		{
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			return { width,height };
		}
		void Window::setFramebufferResizeCallback(const GLFWframebuffersizefun func)
		{
			glfwSetFramebufferSizeCallback(window,func);
		}
		void Window::setSizeLimits(const Size minWidth, const Size minHeight, const Size maxWidth, const Size maxHeight)
		{
			glfwSetWindowSizeLimits(window, minWidth, minHeight, maxWidth, maxHeight);
		}
		void Window::setAspectRatio(const Size horizontal, const Size vertical)
		{
			glfwSetWindowAspectRatio(window, horizontal, vertical);
		}
		mc::Vector2i Window::getPosition() const
		{
			int x, y;
			glfwGetWindowPos(window, &x, &y);
			return { x,y };
		}
		void Window::setPosition(const int x, const int y)
		{
			glfwSetWindowPos(window, x, y);
		}
		void Window::setPositionCallback(const GLFWwindowposfun func)
		{
			glfwSetWindowPosCallback(window, func);
		}
		void Window::setIcons(GLFWimage * pointer, const Size size)
		{
			glfwSetWindowIcon(window, size, pointer);
		}
		GLFWmonitor * Window::getMonitor() const
		{
			return glfwGetWindowMonitor(window);
		}
		void Window::setMonitor(GLFWmonitor* monitor, const int xpos, const int ypos, const Size width, const Size height, const int refreshRate)
		{
			glfwSetWindowMonitor(window, monitor,xpos,ypos,width,height,refreshRate);
		}
		void Window::setIconification(const bool iconify)
		{
			if (iconify) {
				this->iconify();
			}
			else {
				this->restore();
			}
		}
		void Window::setIconificationCallback(const GLFWwindowiconifyfun func)
		{
			glfwSetWindowIconifyCallback(window, func);
		}
		void Window::iconify()
		{
			glfwIconifyWindow(window);
		}
		void Window::restore()
		{
			glfwRestoreWindow(window);
		}
		int Window::isIconified() const
		{
			return getAttribute(GLFW_ICONIFIED);
		}
		int Window::isMaximized() const
		{
			return getAttribute(GLFW_MAXIMIZED);
		}
		int Window::isResizable() const
		{
			return getAttribute(GLFW_RESIZABLE);
		}
		int Window::isDecorated() const
		{
			return getAttribute(GLFW_DECORATED);
		}
		int Window::isFloating() const
		{
			return getAttribute(GLFW_FLOATING);
		}
		void Window::setVisibility(const bool visible)
		{
			if (visible) {
				show();
			}
			else {
				hide();	
			}
		}
		void Window::hide()
		{
			glfwHideWindow(window);
		}
		void Window::show()
		{
			glfwShowWindow(window);
		}
		int Window::isVisible() const
		{
			return getAttribute(GLFW_VISIBLE);
		}
		void Window::setFocusCallback(const GLFWwindowfocusfun func)
		{
			glfwSetWindowFocusCallback(window,func);
		}
		void Window::focus()
		{
			glfwFocusWindow(window);
		}
		void Window::maximize()
		{
			glfwMaximizeWindow(window);
		}
		int Window::isFocused() const
		{
			return getAttribute(GLFW_FOCUSED);
		}
		void Window::setRefreshCallback(const GLFWwindowrefreshfun func)
		{
			glfwSetWindowRefreshCallback(window, func);
		}
		void Window::swapBuffers()
		{
			glfwSwapBuffers(window);
		}
		void Window::setSwapInterval(const int interval)
		{
			glfwSwapInterval(interval);
		}
		void Window::makeContextCurrent()
		{
			glfwMakeContextCurrent(window);
		}
		void Window::setWindowHint(const int name, const int value)
		{
			glfwWindowHint(name, value);
		}
	}
}