/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Core/Constants.h>
#include <MACE/Core/System.h>

#define MACE_EXPOSE_GLFW
#include <MACE/Graphics/Window.h>
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/OGL/OGL.h>
#include <MACE/Graphics/OGL/OGL33Renderer.h>
#include <MACE/Graphics/OGL/OGL33Context.h>

#include <MACE/Utility/BitField.h>

#include <mutex>
#include <ctime>
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <sstream>

//so we can use glew
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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

			GLFWwindow* createWindow(const WindowModule::LaunchConfig& config) {
				GLFWmonitor* mon = nullptr;
				if (config.fullscreen) {
					mon = glfwGetPrimaryMonitor();

					const GLFWvidmode* mode = glfwGetVideoMode(mon);
					glfwWindowHint(GLFW_RED_BITS, mode->redBits);
					glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
					glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
					glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

					return glfwCreateWindow(mode->width, mode->height, config.title, mon, nullptr);
				} else {
					return glfwCreateWindow(config.width, config.height, config.title, nullptr, nullptr);
				}
			}

			///GLFW callback functions

			void onGLFWError(int id, const char* desc) {
				MACE__THROW(Window, "GLFW errored with an ID of " + std::to_string(id) + " and a description of \'" + desc + '\'');
			}

			void onWindowClose(GLFWwindow* window) {
				WindowModule* mod = convertGLFWWindowToModule(window);
				mod->makeDirty();

				const os::WindowModule::LaunchConfig& config = mod->getLaunchConfig();
				config.onClose(*mod);

				if (config.terminateOnClose) {
					mc::MACE::requestStop();
				}
			}

			void onWindowKeyButton(GLFWwindow*, int key, int, int action, int mods) {
				BitField actions = BitField(0);
				actions.setBit(Input::PRESSED, action == GLFW_PRESS);
				actions.setBit(Input::REPEATED, action == GLFW_REPEAT);
				actions.setBit(Input::RELEASED, action == GLFW_RELEASE);
				actions.setBit(Input::MODIFIER_SHIFT, (mods & GLFW_MOD_SHIFT) != 0);
				actions.setBit(Input::MODIFIER_CONTROL, (mods & GLFW_MOD_CONTROL) != 0);
				actions.setBit(Input::MODIFIER_ALT, (mods & GLFW_MOD_ALT) != 0);
				actions.setBit(Input::MODIFIER_SUPER, (mods & GLFW_MOD_SUPER) != 0);

				pushKeyEvent(static_cast<short int>(key), actions);
			}

			void onWindowMouseButton(GLFWwindow*, int button, int action, int mods) {
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
			}

			void onWindowCursorPosition(GLFWwindow* window, double xpos, double ypos) {
				mouseX = static_cast<int>(mc::math::floor(xpos));
				mouseY = static_cast<int>(mc::math::floor(ypos));

				WindowModule* win = convertGLFWWindowToModule(window);
				win->getLaunchConfig().onMouseMove(*win, mouseX, mouseY);
			}

			void onWindowScrollWheel(GLFWwindow* window, double xoffset, double yoffset) {
				scrollY = yoffset;
				scrollX = xoffset;

				WindowModule* win = convertGLFWWindowToModule(window);
				win->getLaunchConfig().onScroll(*win, scrollX, scrollY);
			}

			void onWindowFramebufferResized(GLFWwindow* window, int, int) {
				WindowModule* win = convertGLFWWindowToModule(window);
				win->getContext()->getRenderer()->flagResize();
				win->makeDirty();
			}

			void onWindowDamaged(GLFWwindow* window) {
				convertGLFWWindowToModule(window)->makeDirty();
			}
		}//anon namespace

		WindowModule::WindowModule(const LaunchConfig& c) : config(c) {}

		WindowModule::WindowModule(const int width, const int height, const char * title) : config(LaunchConfig(width, height, title)) {}

		void WindowModule::create() {
			switch (config.contextType) {
				case ContextType::AUTOMATIC:
				case ContextType::BEST_OGL:
				case ContextType::OGL33:
					context = std::shared_ptr<gfx::GraphicsContext>(new gfx::ogl::OGL33Context(this));

					glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
					glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

					glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

					glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
#ifdef MACE_DEBUG
					glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
					break;
			}

			if (context == nullptr) {
				MACE__THROW(NullPointer, "Internal Error: GraphicsContext is nullptr");
			}

			glfwWindowHint(GLFW_RESIZABLE, config.resizable);
			glfwWindowHint(GLFW_DECORATED, config.decorated);

			window = createWindow(config);

			if (config.contextType == ContextType::BEST_OGL || config.contextType == ContextType::AUTOMATIC) {
				int versionMajor = 3;

				//this checks every available context until 1.0. the window is hidden so it won't cause spazzing
				for (int versionMinor = 3; versionMinor >= 0 && !window; --versionMinor) {
					std::cout << "Trying OpenGL " << versionMajor << "." << versionMinor << std::endl;
					glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versionMajor);
					glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versionMinor);

					glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

					window = createWindow(config);
					if (versionMinor == 0 && versionMajor > 1 && !window) {
						glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
						versionMinor = 3;
						--versionMajor;
					}
				}
			}

			if (!window) {
				MACE__THROW(InitializationFailed, "OpenGL context was unable to be created. This graphics card may not be supported or the graphics drivers are installed incorrectly");
			}

			glfwMakeContextCurrent(window);

			gfx::ogl::checkGLError(__LINE__, __FILE__, "Error creating window");

			context->init();

			if (config.vsync)glfwSwapInterval(1);
			else glfwSwapInterval(0);

			glfwSetWindowUserPointer(window, this);

			glfwSetWindowCloseCallback(window, &onWindowClose);
			glfwSetKeyCallback(window, &onWindowKeyButton);
			glfwSetMouseButtonCallback(window, &onWindowMouseButton);
			glfwSetCursorPosCallback(window, &onWindowCursorPosition);
			glfwSetScrollCallback(window, &onWindowScrollWheel);

			glfwSetFramebufferSizeCallback(window, &onWindowFramebufferResized);
			glfwSetWindowRefreshCallback(window, &onWindowDamaged);

			int width = 0, height = 0;

			glfwGetFramebufferSize(window, &width, &height);

			if (width != config.width || height != config.height) {
				context->getRenderer()->resize(width, height);
			}

			config.onCreate(*this);
		}//create 

		const WindowModule::LaunchConfig & WindowModule::getLaunchConfig() const {
			return config;
		}

		void WindowModule::setTitle(const std::string & newTitle) {
			if (!getProperty(gfx::Entity::INIT)) {
				MACE__THROW(InvalidState, "WindowModule not initialized! Must call MACE::init() first!");
			}

			glfwSetWindowTitle(window, newTitle.c_str());
		}

		void WindowModule::threadCallback() {
			try {
				//mutex for this function.
				std::mutex mutex;

				//now is set to be time(nullptr) every loop, and the delta is calculated from now nad last frame.
				time_t now = time(nullptr);
				//each time the frame is swapped, lastFrame is updated with the new time
				time_t lastFrame = time(nullptr);

				//this stores how many milliseconds it takes for the frame to swap.
				float windowDelay = 0;

				try {
					const std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex

					/*create() needs to have this property set to true
					Entity::init() sets it to true
					However, Entity::init() requires create() to be called first, and if
					it detects that this property is set to true, errors because it cant
					be initialized twice. So we have to do this hacky thing to allow this
					to happen
					*/
					setProperty(gfx::Entity::INIT, true);

					create();

					setProperty(gfx::Entity::INIT, false);

					Entity::init();

					if (config.fps != 0) {
						windowDelay = 1000.0f / static_cast<float>(config.fps);
					}
				} catch (const std::exception& e) {
					Error::handleError(e);
				} catch (...) {
					MACE__THROW(InitializationFailed, "An error has occured trying to initalize MACE");
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
								context->getRenderer()->setUp(this);
								Entity::render();
								context->getRenderer()->tearDown(this);
							}

							context->render();

							if (!MACE::isRunning()) {
								break; // while (!MACE::isRunning) would require a lock on destroyed or have it be an atomic varible, both of which are undesirable. while we already have a lock, set a stack variable to false.that way, we only read it, and we dont need to always lock it
							}

						}

						if (windowDelay != 0) {
							now = time(nullptr);

							const time_t delta = now - lastFrame;

							if (delta < windowDelay) {
								lastFrame = now;

								os::wait(static_cast<long long int>(windowDelay - delta));
							}
						}
					} catch (const std::exception& e) {
						Error::handleError(e);
						break;
					} catch (...) {
						MACE__THROW(Unknown, "An unknown error occured trying to render a fraem");
					}
				}

				{
					const std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex
					try {
						Entity::destroy();

						context->destroy();

						glfwDestroyWindow(window);
					} catch (const std::exception& e) {
						Error::handleError(e);
					} catch (...) {
						MACE__THROW(Unknown, "An unknown error occured trying to destroy the rendering thread");
					}
				}
			} catch (const std::exception& e) {
				Error::handleError(e);
				return;
			}
		}//threadCallback

		void WindowModule::init() {
			if (!glfwInit()) {
				MACE__THROW(InitializationFailed, "GLFW failed to initialize!");
			}

			glfwSetErrorCallback(&onGLFWError);

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
			return "MACE/Window#" + std::string(config.title);
		}//getName()

		bool WindowModule::isDestroyed() const {
			return properties.getBit(WindowModule::DESTROYED);
		}
		Vector<int, 2> WindowModule::getFramebufferSize() const {
			if (!getProperty(gfx::Entity::INIT)) {
				MACE__THROW(InvalidState, "WindowModule not initialized! Must call MACE::init() first!");
			}

			Vector<int, 2> out = {};

			glfwGetFramebufferSize(window, &out[0], &out[1]);

			return out;
		}

		std::shared_ptr<gfx::GraphicsContext> WindowModule::getContext() {
			return context;
		}

		const std::shared_ptr<const gfx::GraphicsContext> WindowModule::getContext() const {
			return context;
		}

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
		}//Input

		WindowModule::LaunchConfig::LaunchConfig(const int w, const int h, const char * t) : title(t), width(w), height(h) {}

		bool WindowModule::LaunchConfig::operator==(const LaunchConfig & other) const {
			return title == other.title && width == other.width && height == other.height
				&& fps == other.fps && contextType == other.contextType
				&& onCreate == other.onCreate && onClose == other.onClose
				&& onScroll == other.onScroll && onMouseMove == other.onMouseMove
				&& terminateOnClose == other.terminateOnClose
				&& decorated == other.decorated && fullscreen == other.fullscreen
				&& resizable == other.resizable && vsync == other.vsync;
		}

		bool WindowModule::LaunchConfig::operator!=(const LaunchConfig & other) const {
			return !operator==(other);
		}

		WindowModule * getCurrentWindow() {
			GLFWwindow* win = glfwGetCurrentContext();
			if (win == nullptr) {
				MACE__THROW(NullPointer, "No window in this thread");
			}

			return convertGLFWWindowToModule(win);
		}

		WindowModule * getWindow(const std::string title) {
			Module* mod = MACE::getModule("MACE/Window#" + title);
			return dynamic_cast<WindowModule*>(mod);
		}

		WindowModule * getWindow(const char * title) {
			return getWindow(std::string(title));
		}

		WindowModule * convertGLFWWindowToModule(GLFWwindow * win) {
			if (win == nullptr) {
				MACE__THROW(NullPointer, "Input to convertGLFWWindowToModule is nullptr!");
			}

			WindowModule* windowModule = static_cast<WindowModule*>(glfwGetWindowUserPointer(win));
			if (windowModule == nullptr) {
				MACE__THROW(NullPointer, "No window module found in window");
			}

			return windowModule;
		}

	}//os
}//mc