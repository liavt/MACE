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

namespace mc {
	/**
	Namespace with classes relating to input and windows.
	*/
	namespace win
	{
		class Window {
			friend class WindowModule;
		protected:
			GLFWwindow* m_window;
			int m_originalWidth;
			int m_originalHeight;
			char* m_title;

			unsigned int fps=0;
		public:
			Window(int width, int height, const char* title);
			virtual GLFWwindow* getGLFWWindow();

			const unsigned int& getFPS() const;
			void setFPS(const unsigned int& FPS);

			virtual void create();
			virtual void poll();
			virtual void destroy();
		};
	}
}
