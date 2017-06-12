/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Core/Constants.h>
#include <MACE/Core/System.h>

#include <MACE/Graphics/Window.h>
#include <MACE/Graphics/Renderer.h>

#include <MACE/Utility/BitField.h>

#include <mutex>
#include <ctime>
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <sstream>

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

			GLFWwindow* createWindow(const bool fullscreen, const Size width, const Size height, const std::string& title) {
				GLFWmonitor* mon = nullptr;
				if (fullscreen) {
					mon = glfwGetPrimaryMonitor();

					const GLFWvidmode* mode = glfwGetVideoMode(mon);
					glfwWindowHint(GLFW_RED_BITS, mode->redBits);
					glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
					glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
					glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

					return glfwCreateWindow(mode->width, mode->height, title.c_str(), mon, nullptr);
				} else {
					return glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
				}
			}
		}//anon namespace

		WindowModule::WindowModule(const LaunchConfig& c) : config(c) {}

		WindowModule::WindowModule(const int width, const int height, const char * title) : config(LaunchConfig(width, height, title)) {}

		void WindowModule::create() {
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			glfwWindowHint(GLFW_RESIZABLE, config.resizable);
			glfwWindowHint(GLFW_DECORATED, config.decorated);

#ifdef MACE_DEBUG
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

			window = createWindow(config.fullscreen, config.width, config.height, config.title);

			Index versionMajor = 3;

			//this checks every available context until 1.0. the window is hidden so it won't cause spazzing
			for (int versionMinor = 3; versionMinor >= 0 && !window; --versionMinor) {
				std::cout << "Trying OpenGL " << versionMajor << "." << versionMinor << std::endl;
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versionMajor);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versionMinor);

				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

				window = createWindow(properties.getBit(config.fullscreen), config.width, config.height, config.title);
				if (versionMinor == 0 && versionMajor > 1 && !window) {
					glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
					versionMinor = 3;
					--versionMajor;
				}
			}

			if (!window) {
				throw mc::InitializationFailedError("OpenGL context was unable to be created. This graphics card may not be supported or the graphics drivers are installed incorrectly");
			}

			glfwMakeContextCurrent(window);

			gfx::ogl::checkGLError(__LINE__, __FILE__, "Error creating window");

			glewExperimental = true;
			GLenum result = glewInit();
			if (result != GLEW_OK) {
				std::ostringstream errorMessage;
				errorMessage << "GLEW failed to initialize: ";
				//to convert from GLubyte* to string, we can use the << in ostream. For some reason the
				//+ operater in std::string can not handle this conversion.
				errorMessage << glewGetErrorString(result);

				if (result == GLEW_ERROR_NO_GL_VERSION) {
					errorMessage << "\nThis can be a result of an outdated graphics driver. Please ensure that you have OpenGL 3.0+";
				}
				throw mc::InitializationFailedError(errorMessage.str());
			}

			try {
				gfx::ogl::checkGLError(__LINE__, __FILE__, "Internal Error: This should be ignored silently, it is a bug with glew");
			} catch (...) {
				//glew sometimes throws errors that can be ignored (GL_INVALID_ENUM)
			}

			if (!GLEW_VERSION_3_3) {
				std::cerr << "OpenGL 3.3 not found, falling back to a lower version, which may cause undefined results. Try updating your graphics driver to fix this." << std::endl;
			}

			std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
			std::cout << "OpenGL has been created succesfully!" << std::endl;
			std::cout << "Version: " << std::endl << "	" << glGetString(GL_VERSION) << std::endl;
			std::cout << "Vendor: " << std::endl << "	" << glGetString(GL_VENDOR) << std::endl;
			std::cout << "Renderer: " << std::endl << "	" << glGetString(GL_RENDERER) << std::endl;
			std::cout << "Shader version: " << std::endl << "	" << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
			std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

			if (config.vsync)glfwSwapInterval(1);
			else glfwSwapInterval(0);

			glfwSetWindowUserPointer(window, this);

			const auto closeCallback = [](GLFWwindow* window) {
				WindowModule* mod = static_cast<WindowModule*>(glfwGetWindowUserPointer(window));
				mod->makeDirty();

				const LaunchConfig& config = mod->getLaunchConfig();
				config.onClose(*mod);

				if (config.terminateOnClose) {
					mc::MACE::requestStop();
				}
			};
			glfwSetWindowCloseCallback(window, closeCallback);

			const auto keyDown = [](GLFWwindow*, int key, int, int action, int mods) {
				BitField actions = BitField(0);
				actions.setBit(Input::PRESSED, action == GLFW_PRESS);
				actions.setBit(Input::REPEATED, action == GLFW_REPEAT);
				actions.setBit(Input::RELEASED, action == GLFW_RELEASE);
				actions.setBit(Input::MODIFIER_SHIFT, (mods & GLFW_MOD_SHIFT) != 0);
				actions.setBit(Input::MODIFIER_CONTROL, (mods & GLFW_MOD_CONTROL) != 0);
				actions.setBit(Input::MODIFIER_ALT, (mods & GLFW_MOD_ALT) != 0);
				actions.setBit(Input::MODIFIER_SUPER, (mods & GLFW_MOD_SUPER) != 0);

				pushKeyEvent(static_cast<short int>(key), actions);
			};
			glfwSetKeyCallback(window, keyDown);

			const auto mouseDown = [](GLFWwindow*, int button, int action, int mods) {
				BitField actions = BitField(0);
				actions.setBit(Input::PRESSED, action == GLFW_PRESS);
				actions.setBit(Input::REPEATED, action == GLFW_REPEAT);
				actions.setBit(Input::RELEASED, action == GLFW_RELEASE);
				actions.setBit(Input::MODIFIER_SHIFT, (mods & GLFW_MOD_SHIFT) != 0);
				actions.setBit(Input::MODIFIER_CONTROL, (mods & GLFW_MOD_CONTROL) != 0);
				actions.setBit(Input::MODIFIER_ALT, (mods & GLFW_MOD_ALT) != 0);
				actions.setBit(Input::MODIFIER_SUPER, (mods & GLFW_MOD_SUPER) != 0);

				//in case that we dont have it mapped the same way that GLFW does, we add MOUSE_FIRST which is the offset to the mouse bindings.
				pushKeyEvent(static_cast<short int>(button) + Input::MOUSE_FIRST, actions);
			};
			glfwSetMouseButtonCallback(window, mouseDown);

			const auto cursorPosition = [](GLFWwindow* window, double xpos, double ypos) {
				mouseX = static_cast<int>(mc::math::floor(xpos));
				mouseY = static_cast<int>(mc::math::floor(ypos));

				WindowModule* win = static_cast<WindowModule*>(glfwGetWindowUserPointer(window));
				win->getLaunchConfig().onMouseMove(*win, mouseX, mouseY);
			};
			glfwSetCursorPosCallback(window, cursorPosition);

			const auto scrollWheel = [](GLFWwindow* window, double xoffset, double yoffset) {
				scrollY = yoffset;
				scrollX = xoffset;

				WindowModule* win = static_cast<WindowModule*>(glfwGetWindowUserPointer(window));
				win->getLaunchConfig().onScroll(*win, scrollX, scrollY);
			};
			glfwSetScrollCallback(window, scrollWheel);

			const auto framebufferResize = [](GLFWwindow* window, int, int) {
				gfx::getRenderer()->flagResize();
				static_cast<WindowModule*>(glfwGetWindowUserPointer(window))->makeDirty();
			};
			glfwSetFramebufferSizeCallback(window, framebufferResize);

			const auto windowDamaged = [](GLFWwindow* window) {
				static_cast<WindowModule*>(glfwGetWindowUserPointer(window))->makeDirty();
			};
			glfwSetWindowRefreshCallback(window, windowDamaged);

			gfx::getRenderer()->init(config.width, config.height);

			int width = 0, height = 0;

			glfwGetFramebufferSize(window, &width, &height);

			if (width != config.width || height != config.height) {
				gfx::getRenderer()->resize(width, height);
			}

			config.onCreate(*this);
		}//create 

		const WindowModule::LaunchConfig & WindowModule::getLaunchConfig() const {
			return config;
		}

		GLFWwindow* WindowModule::getGLFWWindow() {
			return window;
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
				const std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex

				create();

				Entity::init();

				if (config.fps != 0.0f) {
					windowDelay = 1000.0f / static_cast<float>(config.fps);
				}
			} catch (const std::exception& e) {
				Error::handleError(e);
			} catch (...) {
				std::cerr << "An error has occured trying to initalize MACE";
				MACE::requestStop();
			}

			//this is the main rendering loop.
			//we loop infinitely until break is called. break is called when an exception is thrown or MACE::isRunning is false
			for (;;) {//( ;_;)
				try {

					{
						//thread doesn't own window, so we have to lock the mutex
						const std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex

						glfwPollEvents();

						if (getProperty(Entity::DIRTY)) {
							gfx::getRenderer()->clearBuffers();

							Entity::render();

							gfx::getRenderer()->renderFrame(this);
						}

						gfx::getRenderer()->checkInput(this);

						if (!MACE::isRunning()) {
							break; // while (!MACE::isRunning) would require a lock on destroyed or have it be an atomic varible, both of which are undesirable. while we already have a lock, set a stack variable to false.that way, we only read it, and we dont need to always lock it
						}

					}
					now = time(0);

					const time_t delta = now - lastFrame;

					if (delta < windowDelay) {
						lastFrame = now;

						os::wait(static_cast<long long int>(windowDelay));
					}
				} catch (const std::exception& e) {
					Error::handleError(e);
					break;
				} catch (...) {
					std::cerr << "An unknown error has occured trying to render MACE";
					MACE::requestStop();
					break;
				}
			}

			{
				const std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex
				try {
					Entity::destroy();

					gfx::getRenderer()->destroy();

					glfwDestroyWindow(window);
				} catch (const std::exception& e) {
					Error::handleError(e);
				} catch (...) {
					std::cerr << "An unknown error has occured trying to destroy MACE";
					MACE::requestStop();
				}
			}

		}//threadCallback

		void WindowModule::init() {
			if (!glfwInit()) {
				throw InitializationFailedError("GLFW failed to initialize!");
			}

			const auto errorCallback = [](int id, const char* desc) {
				throw gfx::ogl::OpenGLError("GLFW errored with an ID of " + std::to_string(id) + " and a description of \'" + desc + '\'');
			};
			glfwSetErrorCallback(errorCallback);

			windowThread = std::thread(&WindowModule::threadCallback, this);
		}

		void WindowModule::update() {
			std::mutex mutex;
			const std::unique_lock<std::mutex> guard(mutex);

			Entity::update();
		}//update

		void WindowModule::destroy() {
			{
				std::mutex mutex;
				const std::unique_lock<std::mutex> guard(mutex);
				setProperty(WindowModule::DESTROYED, true);
			}

			windowThread.join();

			glfwTerminate();
		}//destroy

		std::string WindowModule::getName() const {
			return "MACE/Window";
		}//getName()

		bool WindowModule::isDestroyed() const {
			return properties.getBit(WindowModule::DESTROYED);
		}//isDestroyed

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
				return keys[key].getBit(Input::PRESSED) || keys[key].getBit(Input::REPEATED);
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
		}

		WindowModule::LaunchConfig::LaunchConfig(const int w, const int h, const char * t) : title(t), width(w), height(h) {}
		//Input
	}//os
}//mc
