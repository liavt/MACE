/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Window.h>
#include <MACE/System/Constants.h>
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

			int mouseX = -1;
			int mouseY = -1;

			double scrollX = 0;
			double scrollY = 0;

			void pushKeyEvent(const short int& key, const BitField action) {
				keys[key] = action;
			}
		}//anon namespace

		WindowModule::WindowModule(const int width, const int height, const char* windowTitle) : title(windowTitle), originalWidth(width), originalHeight(height) {}

		void WindowModule::create() {
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			window = glfwCreateWindow(originalWidth, originalHeight, title.c_str(), NULL, NULL);

			//first we set up glew and opengl
			glfwMakeContextCurrent(window);

            glewExperimental = true;
			GLenum result = glewInit();
			if( result != GLEW_OK ) {
				throw mc::InitializationError("GLEW failed to initialize with result " + std::to_string(result));
			}

			if( GL_VERSION_3_0 ) {
				std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
				std::cout << "OpenGL has been created succesfully!" << std::endl;
				std::cout << "Version: " << std::endl << "	" << glGetString(GL_VERSION) << std::endl;
				std::cout << "Vendor: " << std::endl << "	" << glGetString(GL_VENDOR) << std::endl;
				std::cout << "Renderer: " << std::endl << "	" << glGetString(GL_RENDERER) << std::endl;
				std::cout << "Shader version: " << std::endl << "	" << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
				std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
			} else if( !GL_VERSION_3_3 ) {
				std::cerr << "OpenGL 3.3 not found, falling back to OpenGL 3.0, which may cause undefined results. Try updating your graphics driver to fix this.";
			} else {
				throw mc::InitializationError("OpenGL 3.0+ not found");
			}

			gfx::ogl::checkGLError();

			if( vsync )glfwSwapInterval(1);
			else glfwSwapInterval(0);

			glfwSetWindowUserPointer(window, this);

			auto closeCallback = [] (GLFWwindow* window) {
				static_cast<WindowModule*>(glfwGetWindowUserPointer(window))->makeDirty();

				mc::System::requestStop();
			};
			glfwSetWindowCloseCallback(window, closeCallback);

			auto keyDown = [] (GLFWwindow*, int key, int, int action, int mods) {
				BitField actions = BitField(0);
				actions.setBit(Input::PRESSED, action == GLFW_PRESS);
				actions.setBit(Input::REPEATED, action == GLFW_REPEAT);
				actions.setBit(Input::RELEASED, action == GLFW_RELEASE);
				actions.setBit(Input::MOD_SHIFT, (mods & GLFW_MOD_SHIFT) != 0);
				actions.setBit(Input::MOD_CONTROL, (mods & GLFW_MOD_CONTROL) != 0);
				actions.setBit(Input::MOD_ALT, (mods & GLFW_MOD_ALT) != 0);
				actions.setBit(Input::MOD_SUPER, (mods & GLFW_MOD_SUPER) != 0);

				pushKeyEvent(static_cast<short int>(key), actions);
			};
			glfwSetKeyCallback(window, keyDown);

			auto mouseDown = [] (GLFWwindow*, int button, int action, int mods) {
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
			};
			glfwSetMouseButtonCallback(window, mouseDown);

			auto cursorPosition = [] (GLFWwindow*, double xpos, double ypos) {
				mouseX = static_cast<int>(mc::math::floor(xpos));
				mouseY = static_cast<int>(mc::math::floor(ypos));
			};
			glfwSetCursorPosCallback(window, cursorPosition);

			auto scrollWheel = [] (GLFWwindow*, double xoffset, double yoffset) {
				scrollY = yoffset;
				scrollX = xoffset;
			};
			glfwSetScrollCallback(window, scrollWheel);

			auto framebufferResize = [] (GLFWwindow* window, int width, int height) {
				gfx::Renderer::resize(width, height);
				static_cast<WindowModule*>(glfwGetWindowUserPointer(window))->makeDirty();
			};
			glfwSetFramebufferSizeCallback(window, framebufferResize);

			auto windowDamaged = [] (GLFWwindow* window) {
				static_cast<WindowModule*>(glfwGetWindowUserPointer(window))->makeDirty();
			};
			glfwSetWindowRefreshCallback(window, windowDamaged);

			int width = 0, height = 0;

			glfwGetFramebufferSize(window, &width, &height);

			gfx::Renderer::init(width, height);

			creationCallback();
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

			//this stores how many milliseconds it takes for the frame to swap. it is 1 by default so it doesnt create an infinite loop
			float windowDelay = 0;


			try {
				std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex

                create();

				Entity::init();

				if( fps != 0.0f ) {
					windowDelay = 1000.0f / static_cast<float>(fps);
				}
			} catch( const std::exception& e ) {
				Exception::handleException(e);
			} catch( ... ) {
				std::cerr << "An error has occured";
				System::requestStop();
			}

			//this is the main rendering loop.
			//we loop infinitely until break is called. break is called when an exception is thrown or System::isRunning is false
			for( ;;) {//( ;_;)
				try {

					{
						//thread doesn't own window, so we have to lock the mutex
						std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex

						glfwPollEvents();

						if( getProperty(Entity::DIRTY) ) {
							gfx::Renderer::clearBuffers();

							Entity::render();

							gfx::Renderer::renderFrame(this);
						}

						gfx::Renderer::checkInput();

						if( !System::isRunning() ) {
							break; // while (!System::isRunning) would require a lock on destroyed or have it be an atomic varible, both of which are undesirable. while we already have a lock, set a stack variable to false.that way, we only read it, and we dont need to always lock it
						}

					}
					now = time(0);

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

		void WindowModule::setCreationCallback(const VoidFunctionPtr callback) {
			creationCallback = callback;
		}//setCreationCallback

		const VoidFunctionPtr WindowModule::getCreationCallback() const {
			return creationCallback;
		}//getCreationCallback

		VoidFunctionPtr WindowModule::getCreationCallback() {
			return creationCallback;
		}//getCreationCallback

		void WindowModule::onInit() {}

		void WindowModule::onUpdate() {}

		void WindowModule::onRender() {}

		void WindowModule::onDestroy() {}

		void WindowModule::clean() {
			setProperty(Entity::DIRTY, false);
		}//clean()

		namespace Input {
			const BitField & getKey(const short int key) {
				return keys[key];
			}

			bool isKeyDown(const short int key) {
				return keys[key].getBit(Input::PRESSED)||keys[key].getBit(Input::REPEATED);
			}

			bool isKeyRepeated(const short int key) {
				return keys[key].getBit(Input::REPEATED);
			}

			bool isKeyReleased(const short int key) {
				return keys[key].getBit(Input::RELEASED);
			}
			int getMouseX() noexcept {
				return mouseX;
			}
			int getMouseY() noexcept {
				return mouseY;
			}
			double getScrollVertical() noexcept {
				return scrollY;
			}
			double getScrollHorizontal() noexcept {
				return scrollX;
			}
		}//Input
	}//os
}//mc
