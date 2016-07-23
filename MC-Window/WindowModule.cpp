/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MC-Window/WindowModule.h>

namespace mc {
	namespace win
	{
		WindowModule::WindowModule(Window* win) {
			this->window = win;

		}
		void WindowModule::init() {
			window->create();
		}

		void WindowModule::update() {
			if (window->poll()) System::requestStop();
			
		}

		void WindowModule::destroy() {
			window->destroy();
		}

		std::string WindowModule::getName() const {
			return "MC-Window";
		}

	}
}