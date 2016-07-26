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

namespace mc {
	namespace gfx{
		void GraphicsModule::graphicsThreadCallback()
		{
			std::lock_guard<std::mutex> guard(mutex);

			try {

				SDL_Window* sdlwin = window->getSDLWindow();
				SDL_GLContext context = SDL_GL_CreateContext(sdlwin);
				SDL_GL_MakeCurrent(sdlwin, context);

				while (!destroyed) {
					//	std::cout << "hello!"<<std::endl;
					SDL_Delay(10);
					SDL_GL_SwapWindow(window->getSDLWindow());
				}

				SDL_GL_DeleteContext(context);
			}
			catch (std::exception e) {
				std::cout << e.what();
				destroyed = true;
			}
		}
		GraphicsModule::GraphicsModule(win::Window * window)
		{
			this->window = window;
		}
		void GraphicsModule::init() {
			System::assertModule("MC-Window");
			win::Window* win = this->window;
			graphicsThread = std::thread(&GraphicsModule::graphicsThreadCallback, this);
		}

		void GraphicsModule::update() {
			std::unique_lock<std::mutex> guard(mutex);
			EntityModule::update();
			guard.unlock();//if update() is called in a different function, the unique_lock wont be released. here its explicetely unloacked
		}

		void GraphicsModule::destroy() {
			mutex.lock();
			destroyed = true;
			mutex.unlock();
			graphicsThread.join();
		}

		std::string GraphicsModule::getName() const{
			return "MC-Graphics";
		}
	}
}