/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MC-Window/WindowModule.h>
#include <mutex>
#include <ctime>
#include <chrono>
#include <iostream>

namespace mc {
	namespace win
	{
		void GraphicsContext::init(Window * win)
		{
			//do nothing if no context is defined
		}

		void GraphicsContext::render(Window * win)
		{
			//do nothing if no context is defined
		}

		void GraphicsContext::destroy(Window * win)
		{
			//do nothing if no context is defined
		}

		void GraphicsContext::update()
		{
			//do nothing if no context is defined
		}

		void WindowModule::setContext(GraphicsContext * con)
		{
			if(System::getFlag(SYSTEM_FLAG_INIT)){
				throw InitializationError("Cannot set a GraphicsContext after init() has been called!");
			}
			context = con;
		}

		GraphicsContext* WindowModule::getContext()
		{
			return context;
		}

		const GraphicsContext* WindowModule::getContext() const
		{
			return context;
		}

		void WindowModule::threadCallback() {
			std::mutex mutex;

			bool isRunning = true;

			time_t now = time(0);
			time_t lastFrame = time(0);

			float windowDelay=0;

			try {
				std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex

				window->create();

				if (context != 0) {
					context->init(window);
				}

				if (window->fps != 0) {
					windowDelay = 1000.0f / (float)window->fps;
				}
			}
			catch (const std::exception& e) {
				Exception::handleException(e);
			}
			catch (...) {
				std::cerr << "An error has occured";
				System::requestStop();
			}

			//this is the main rendering loop.
			while (isRunning) {
				try {
					now = time(0);

					{
						std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex

						//thread doesn't own window, so we have to lock the mutex
						window->poll();

						if (context != 0) {
							context->render(window);
						}
							
						isRunning = System::isRunning();//while(!System::isRunning) would require a lock on destroyed or have it be an atomic varible, both of which are undesirable. while we already have a lock, set a stack variable to false. that way, we only read it, and we dont need to always lock it
	
					}

					const time_t delta = now - lastFrame;

					if (delta < windowDelay) {
						lastFrame = now;

						std::this_thread::sleep_for(std::chrono::milliseconds((unsigned int)windowDelay));
					}
				}
				catch (const std::exception& e) {
					Exception::handleException(e);
				}
				catch (...) {
					std::cerr << "An error has occured";
					System::requestStop();
				}
			}

			{
				std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex
				try {
					if (context != 0)context->destroy(window);

					window->destroy();
				}
				catch (const std::exception& e) {
					Exception::handleException(e);
				}
				catch (...) {
					std::cerr << "An error has occured";
					System::requestStop();
				}
			}
		}

		WindowModule::WindowModule(Window* win) {
			this->window = win;

		}
		void WindowModule::init() {
			windowThread = std::thread(&WindowModule::threadCallback,this);
		}

		void WindowModule::update() {
			std::mutex mutex;
			std::unique_lock<std::mutex> guard(mutex);
			if (context != 0)context->update();
			guard.unlock();
		}

		void WindowModule::destroy() {
			destroyed = true;
			windowThread.join();
		}

		std::string WindowModule::getName() const {
			return "MC-Window";
		}

	}
}