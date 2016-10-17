/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once

//so we can use glew
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>
#include <MACE/System/Utility/Vector.h>

namespace mc {
	/**
	Namespace with classes relating to input and windows.
	*/
	namespace win
	{
		class Window {
			friend class WindowModule;
		protected:
			GLFWwindow* window;
			int originalWidth;
			int originalHeight;
			std::string title;

			unsigned int fps=0;
		public:
			Window(const int width, const int height, const char* title);
			virtual GLFWwindow* getGLFWWindow();

			const unsigned int& getFPS() const;
			void setFPS(const unsigned int& FPS);

			virtual void create();
			virtual void poll();
			virtual void destroy();

			const int getOriginalWidth() const;
			const int getOriginalHeight() const;

			std::string getTitle();
			const std::string getTitle() const;
			void setTitle(const std::string& newTitle);

			void* getUserPointer() const;
			void setUserPointer(void* pointer);

			int shouldClose() const;
			void setShouldClose(int value);
			void setCloseCallback(const GLFWwindowclosefun func);

			mc::Vector2i getSize() const;
			mc::Vector4i getFrameSize() const;
			void setSize(const Size width, const Size height);
			void setResizeCallback(const GLFWwindowsizefun func);

			mc::Vector2i getFramebufferSize() const;
			void setFramebufferResizeCallback(GLFWframebuffersizefun func);

			void setSizeLimits(const Size minWidth, const Size minHeight, const Size maxWidth, const Size maxHeight);
			void setAspectRatio(const Size horizontal, const Size vertical);

			mc::Vector2i getPosition() const;
			void setPosition(const int x, const int y);
			void setPositionCallback(const GLFWwindowposfun func);

			void setIcons(GLFWimage* pointer, const Size size);

			GLFWmonitor* getMonitor() const;
			void setMonitor(GLFWmonitor* monitor, const int xpos, const int ypos, const Size width, const Size height, const int refreshRate);

			void setIconification(const bool iconify);
			void setIconificationCallback(const GLFWwindowiconifyfun func);
			void iconify();
			void restore();

			void setVisibility(const bool visible);
			void hide();
			void show();

			void setFocusCallback(const GLFWwindowfocusfun func);
			void focus();

			void maximize();

			void setRefreshCallback(const GLFWwindowrefreshfun func);

			void swapBuffers();

			void makeContextCurrent();

			int getAttribute(const int attribute) const;

			int isFocused() const;
			int isVisible() const;
			int isIconified() const;
			int isMaximized() const;
			int isResizable() const;
			int isDecorated() const;
			int isFloating() const;

			static void setSwapInterval(const int interval);

			static void setWindowHint(const int name, const int value);
		};
	}
}
