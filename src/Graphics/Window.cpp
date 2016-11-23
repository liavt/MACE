/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Window.h>
#include <MACE/System/Constants.h>
#include <MACE/Graphics/GraphicsConstants.h>
#include <MACE/Graphics/Renderer.h>
#include <mutex>
#include <ctime>
#include <chrono>
#include <iostream>
#include <GLFW/glfw3.h>
#include <GL/glew.h>

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

		OpenGLContext::OpenGLContext() : Entity(), GraphicsContext() {

		}
		void OpenGLContext::update() {
			//	SDL_GL_MakeCurrent(window->getSDLWindow(), context);

			std::mutex mutex;
			std::unique_lock<std::mutex> lock(mutex);

			Entity::update();
		}

		void OpenGLContext::setUpWindow(Window * win) {
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}

		void OpenGLContext::init(Window * win) {

			std::mutex mutex;
			std::unique_lock<std::mutex> lock(mutex);

			glfwMakeContextCurrent(win->getGLFWWindow());

			GLenum result = glewInit();
			if( result != GLEW_OK ) {
				throw mc::InitializationError("GLEW failed to initialize with result " + std::to_string(result));
			}

			if( GL_VERSION_1_1 ) {
				std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
				std::cout << "OpenGL has been created succesfully!" << std::endl;
				std::cout << "Version: " << std::endl << "	" << glGetString(GL_VERSION) << std::endl;
				std::cout << "Vendor: " << std::endl << "	" << glGetString(GL_VENDOR) << std::endl;
				std::cout << "Renderer: " << std::endl << "	" << glGetString(GL_RENDERER) << std::endl;
				std::cout << "Shader version: " << std::endl << "	" << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
				std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
			} else {
				throw InitializationError("Error retrieving GLEW version!");
			}

			if( !GL_VERSION_3_0 ) {
				throw InitializationError("OpenGL 3+ is not available!");
			} else if( !GL_VERSION_3_3 ) {
				std::cout << "OpenGL 3.3 not found, falling back to OpenGL 3.0.";
			}

			checkGLError();

			if( vsync )glfwSwapInterval(1);
			else glfwSwapInterval(0);


			auto framebufferResize = [] (GLFWwindow* win, int width, int height) {
				Renderer::resize(width, height);
			};
			glfwSetFramebufferSizeCallback(win->getGLFWWindow(), framebufferResize);

			int width = 0, height = 0;

			glfwGetFramebufferSize(win->getGLFWWindow(), &width, &height);

			Renderer::init(width, height);

			Entity::init();


		}

		void OpenGLContext::render(Window* win) {
			std::mutex mutex;
			std::unique_lock<std::mutex> lock(mutex);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Entity::render();

			Renderer::renderFrame(win);

			glfwSwapBuffers(win->getGLFWWindow());
		}

		void OpenGLContext::destroy(Window* win) {
			std::mutex mutex;
			std::unique_lock<std::mutex> lock(mutex);

			Entity::destroy();

			Renderer::destroy();

		}

		void OpenGLContext::setVSync(const bool & sync) {
			vsync = sync;
		}

		void OpenGLContext::customUpdate() {}

		void OpenGLContext::customRender() {}

		void OpenGLContext::customDestroy() {}

		void OpenGLContext::customInit() {}

	}
}
