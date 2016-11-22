/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Window.h>
#include <MACE/System/Exceptions.h>
#include <mutex>
#include <ctime>
#include <chrono>
#include <iostream>
#include <GLFW/glfw3.h>

namespace mc {
	namespace gfx {
		Window::Window(const int width, const int height, const char* windowTitle) : title(windowTitle), originalWidth(width), originalHeight(height) {}

		void Window::create() {
			window = glfwCreateWindow(originalWidth, originalHeight, title.c_str(), NULL, NULL);

			auto closeCallback = [] (GLFWwindow* win) {mc::System::requestStop(); };
			glfwSetWindowCloseCallback(window, closeCallback);
		}

		void Window::poll() {
			glfwPollEvents();
		}

		GLFWwindow* Window::getGLFWWindow() {
			return window;
		}

		const unsigned int & Window::getFPS() const {
			return fps;
		}

		void Window::setFPS(const unsigned int & FPS) {
			fps = FPS;
		}

		void Window::destroy() {
			glfwDestroyWindow(window);
		}
		const int Window::getOriginalWidth() const {
			return originalWidth;
		}
		const int Window::getOriginalHeight() const {
			return originalHeight;
		}
		std::string Window::getTitle() {
			return title;
		}
		const std::string Window::getTitle() const {
			return title;
		}
		void Window::setTitle(const std::string & newTitle) {
			glfwSetWindowTitle(window, newTitle.c_str());
		}
		void WindowModule::setContext(GraphicsContext * con) {
			if( System::getFlag(SYSTEM_FLAG_INIT) ) {
				throw InitializationError("Cannot set a GraphicsContext after init() has been called!");
			}
			context = con;
		}

		GraphicsContext* WindowModule::getContext() {
			return context;
		}

		const GraphicsContext* WindowModule::getContext() const {
			return context;
		}

		void WindowModule::threadCallback() {
			//mutex for this function.
			std::mutex mutex;

			//now is set to be time(0) every loop, and the delta is calculated from now nad last frame.
			time_t now = time(0);
			//each time the frame is swapped, lastFrame is updated with the new time
			time_t lastFrame = time(0);

			//this stores how many milliseconds it takes for the frame to swap.
			float windowDelay = 0;

			try {
				std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex

				if( context != nullptr ) {
					context->setUpWindow(window);
				}

				window->create();

				if( context != nullptr ) {
					context->init(window);
				}

				if( window->fps != 0 ) {
					windowDelay = 1000.0f / (float) window->fps;
				}
			} catch( const std::exception& e ) {
				Exception::handleException(e);
			} catch( ... ) {
				std::cerr << "An error has occured";
				System::requestStop();
			}

			//this is the main rendering loop.
			//we loop infinitely until break is called. break is called when an exception is thrown or System::isRunning is false
			for( ;;) {//;_;
				try {
					now = time(0);

					{
						std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex

						//thread doesn't own window, so we have to lock the mutex
						window->poll();

						if( context != nullptr ) {
							context->render(window);
						}

						if( !System::isRunning() )break; // while (!System::isRunning) would require a lock on destroyed or have it be an atomic varible, both of which are undesirable. while we already have a lock, set a stack variable to false.that way, we only read it, and we dont need to always lock it

					}

					const time_t delta = now - lastFrame;

					if( delta < windowDelay ) {
						lastFrame = now;

						std::this_thread::sleep_for(std::chrono::milliseconds((unsigned int) windowDelay));
					}
				} catch( const std::exception& e ) {
					Exception::handleException(e);
					break;
				} catch( ... ) {
					std::cerr << "An error has occured";
					System::requestStop();
					break;
				}
			}

			{
				std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex
				try {
					if( context != nullptr )context->destroy(window);

					window->destroy();
				} catch( const std::exception& e ) {
					Exception::handleException(e);
				} catch( ... ) {
					std::cerr << "An error has occured";
					System::requestStop();
				}
			}
		}

		WindowModule::WindowModule(Window* win) {
			this->window = win;

		}
		void WindowModule::init() {
			if( !glfwInit() ) {
				throw InitializationError("GLFW failed to initialize!");
			}

			windowThread = std::thread(&WindowModule::threadCallback, this);
		}

		void WindowModule::update() {
			std::mutex mutex;
			std::unique_lock<std::mutex> guard(mutex);
			if( context != 0 )context->update();
			guard.unlock();
		}

		void WindowModule::destroy() {
			destroyed = true;
			windowThread.join();

			glfwTerminate();
		}

		std::string WindowModule::getName() const {
			return "MACE/Window";
		}

	}
}
