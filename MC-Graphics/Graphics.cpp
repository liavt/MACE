/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MC-Graphics/Graphics.h>

namespace mc {
	namespace gfx{
		GraphicsModule::GraphicsModule(win::Window * window)
		{
			System::assertModule("MC-Window");
			this->window = window;

		}
		void GraphicsModule::init() {
			SDL_Window* sdlwin = window->getSDLWindow();
			context = SDL_GL_CreateContext(sdlwin);
			SDL_GL_MakeCurrent(sdlwin, context);
		}

		void GraphicsModule::update() {
			SDL_GL_SwapWindow(window->getSDLWindow());
		}

		void GraphicsModule::destroy() {
			SDL_GL_DeleteContext(context);
		}

		std::string GraphicsModule::getName() const{
			return "MC-Graphics";
		}
	}
}