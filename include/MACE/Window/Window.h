/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

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
			GLFWwindow* getGLFWWindow();

			const unsigned int& getFPS() const;
			void setFPS(const unsigned int& FPS);

			void create();
			void poll();
			void destroy();

			const int getOriginalWidth() const;
			const int getOriginalHeight() const;

			std::string getTitle();
			const std::string getTitle() const;
			void setTitle(const std::string& newTitle);
		};
	}
}
