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
#include <MACE/Utility/BitField.h>
#include <mutex>
#include <ctime>
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <GLFW/glfw3.h>
#include <GL/glew.h>

namespace mc {
	namespace os {
		namespace {
			std::unordered_map< short int, BitField > keys = std::unordered_map< short int, BitField >();

			void pushKeyEvent(const short int& key, const BitField action) {
				keys[key] = action;
			}
		}

		WindowModule::WindowModule(const int width, const int height, const char* windowTitle) : title(windowTitle), originalWidth(width), originalHeight(height) {}

		void WindowModule::create() {

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			window = glfwCreateWindow(originalWidth, originalHeight, title.c_str(), NULL, NULL);

			//first we set up glew and opengl
			glfwMakeContextCurrent(window);

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

			gfx::checkGLError();

			if( vsync )glfwSwapInterval(1);
			else glfwSwapInterval(0);

			glfwSetWindowUserPointer(window, this);

			auto closeCallback = [] (GLFWwindow* window) {
				static_cast<WindowModule*>(glfwGetWindowUserPointer(window))->setProperty(Entity::DIRTY, true);

				mc::System::requestStop();
			};
			glfwSetWindowCloseCallback(window, closeCallback);

			auto keyDown = [] (GLFWwindow* window, int key, int, int action, int mods) {
				BitField actions = BitField(0);
				actions.setBit(Input::PRESSED, action == GLFW_PRESS);
				actions.setBit(Input::REPEATED, action == GLFW_REPEAT);
				actions.setBit(Input::RELEASED, action == GLFW_RELEASE);
				actions.setBit(Input::MOD_SHIFT, (mods & GLFW_MOD_SHIFT) != 0);
				actions.setBit(Input::MOD_CONTROL, (mods & GLFW_MOD_CONTROL) != 0);
				actions.setBit(Input::MOD_ALT, (mods & GLFW_MOD_ALT) != 0);
				actions.setBit(Input::MOD_SUPER, (mods & GLFW_MOD_SUPER) != 0);

				pushKeyEvent(static_cast<short int>(key), actions);

				static_cast<WindowModule*>(glfwGetWindowUserPointer(window))->setProperty(Entity::DIRTY, true);
			};
			glfwSetKeyCallback(window, keyDown);

			auto mouseDown = [] (GLFWwindow* window, int button, int action, int mods) {
				BitField actions = BitField(0);
				actions.setBit(Input::PRESSED, action == GLFW_PRESS);
				actions.setBit(Input::REPEATED, action == GLFW_REPEAT);
				actions.setBit(Input::RELEASED, action == GLFW_RELEASE);
				actions.setBit(Input::MOD_SHIFT, (mods & GLFW_MOD_SHIFT) != 0);
				actions.setBit(Input::MOD_CONTROL, (mods & GLFW_MOD_CONTROL) != 0);
				actions.setBit(Input::MOD_ALT, (mods & GLFW_MOD_ALT) != 0);
				actions.setBit(Input::MOD_SUPER, (mods & GLFW_MOD_SUPER) != 0);

				//in case that we dont have it mapped the same way that GLFW does, we add MOUSE_FIRST which is the offset to the mouse bindings.
				pushKeyEvent(static_cast<short int>(button) + Input::MOUSE_FIRST, actions);

				static_cast<WindowModule*>(glfwGetWindowUserPointer(window))->setProperty(Entity::DIRTY, true);
			};
			glfwSetMouseButtonCallback(window, mouseDown);

			auto framebufferResize = [] (GLFWwindow* window, int width, int height) {
				gfx::Renderer::resize(width, height);
				static_cast<WindowModule*>(glfwGetWindowUserPointer(window))->setProperty(Entity::DIRTY, true);
			};
			glfwSetFramebufferSizeCallback(window, framebufferResize);

			auto windowDamaged = [] (GLFWwindow* window) {
				static_cast<WindowModule*>(glfwGetWindowUserPointer(window))->setProperty(Entity::DIRTY, true);
			};
			glfwSetWindowRefreshCallback(window, windowDamaged);

			int width = 0, height = 0;

			glfwGetFramebufferSize(window, &width, &height);

			gfx::Renderer::init(width, height);
		}//create

		GLFWwindow* WindowModule::getGLFWWindow() {
			return window;
		}

		const unsigned int & WindowModule::getFPS() const {
			return fps;
		}

		void WindowModule::setFPS(const unsigned int & FPS) {
			fps = FPS;
		}

		const int WindowModule::getOriginalWidth() const {
			return originalWidth;
		}
		const int WindowModule::getOriginalHeight() const {
			return originalHeight;
		}
		std::string WindowModule::getTitle() {
			return title;
		}
		const std::string WindowModule::getTitle() const {
			return title;
		}
		void WindowModule::setTitle(const std::string & newTitle) {
			glfwSetWindowTitle(window, newTitle.c_str());
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

				create();

				Entity::init();

				if( fps != 0 ) {
					windowDelay = 1000.0f / (float) fps;
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
						//thread doesn't own window, so we have to lock the mutex
						std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex

						if( getProperty(Entity::DIRTY) ) {
							glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

							Entity::render();

							gfx::Renderer::renderFrame(this);

							glfwSwapBuffers(window);
						}

						glfwPollEvents();

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
					Entity::destroy();

					gfx::Renderer::destroy();

					glfwDestroyWindow(window);
				} catch( const std::exception& e ) {
					Exception::handleException(e);
				} catch( ... ) {
					std::cerr << "An error has occured";
					System::requestStop();
				}
			}
		}//threadCallback
	
		void WindowModule::init() {
			if( !glfwInit() ) {
				throw InitializationError("GLFW failed to initialize!");
			}

			windowThread = std::thread(&WindowModule::threadCallback, this);
		}

		void WindowModule::update() {
			std::mutex mutex;
			std::unique_lock<std::mutex> guard(mutex);

			//glfw only polls the callback when the mouse state changes. if the mouse button is held down, the callback is called once, so we check the cached state if it is held down.
			for( Index i = Input::MOUSE_FIRST; i < Input::MOUSE_LAST; ++i ) {
				if( Input::isKeyDown(i) ) {
					setProperty(Entity::DIRTY, true);
				}
			}

			Entity::update();
		}//update

		void WindowModule::destroy() {
			destroyed = true;
			windowThread.join();

			glfwTerminate();
		}//destroy

		std::string WindowModule::getName() const {
			return "MACE/Window";
		}//getName()

		void WindowModule::setVSync(const bool & sync) {
			vsync = sync;
		}//setVSync

		bool WindowModule::isVSync() const {
			return vsync;
		}//isVSync

		void WindowModule::customInit() {}

		void WindowModule::customUpdate() {}

		void WindowModule::customRender() {}

		void WindowModule::customDestroy() {}

		namespace Input {
			const BitField & getKey(const short int key) {
				return keys[key];
			}

			bool isKeyDown(const short int key) {
				return keys[key].getBit(Input::PRESSED);
			}

			bool isKeyRepeated(const short int key) {
				return keys[key].getBit(Input::REPEATED);
			}

			bool isKeyReleased(const short int key) {
				return keys[key].getBit(Input::RELEASED);
			}
		}//Input
	}//os
}//mc
