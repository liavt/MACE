/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MC-Graphics/Graphics.h>
#include <thread>
#include <iostream>
#include <mutex>
#include <GL/glew.h>

namespace mc {
	namespace gfx{
		OpenGLContext::OpenGLContext() : Container(),GraphicsContext()
		{

		}
		void OpenGLContext::update() {
		//	SDL_GL_MakeCurrent(window->getSDLWindow(), context);

			std::mutex mutex;
			std::unique_lock<std::mutex> lock(mutex);

			updateChildren();

			lock.unlock();

			SDL_Delay(10);
		}


		void OpenGLContext::init(win::Window * win)
		{
			context = SDL_GL_CreateContext(win->getSDLWindow());
			SDL_GL_MakeCurrent(win->getSDLWindow(),context);
			initChildren();
		}

		void OpenGLContext::render(win::Window* win) {
			glClear(GL_COLOR_BUFFER_BIT);

			renderChildren();

			SDL_GL_SwapWindow(win->getSDLWindow());
		}

		void OpenGLContext::destroy(win::Window* win) {
			destroyChildren();
			SDL_GL_DeleteContext(context);
		}
	}
}