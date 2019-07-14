/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Core/Constants.h>
#include <MACE/Core/System.h>
#include <MACE/Utility/Color.h>

#define MACE_EXPOSE_GLFW
#include <MACE/Graphics/Window.h>
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/OGL/OGL33.h>
#include <MACE/Graphics/OGL/OGL33Renderer.h>
#include <MACE/Graphics/OGL/OGL33Context.h>

#include <mutex>
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <sstream>

#include <GLFW/glfw3.h>

namespace mc {
	namespace gfx {
		namespace {
			std::unordered_map< Input::KeyCode, Byte > keys = std::unordered_map< Input::KeyCode, Byte >();

			int mouseX = -1;
			int mouseY = -1;

			double scrollX = 0;
			double scrollY = 0;

			void pushKeyEvent(const Input::KeyCode key, const Byte action) {
				keys[key] = action;
			}

			GLFWwindow* createWindow(const WindowModule::LaunchConfig& config) {
				if (config.fullscreen) {
					GLFWmonitor* mon = glfwGetPrimaryMonitor();

					const GLFWvidmode* mode = glfwGetVideoMode(mon);
					glfwWindowHint(GLFW_RED_BITS, mode->redBits);
					glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
					glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
					glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

					return glfwCreateWindow(mode->width, mode->height, config.title, mon, nullptr);
				} else {
					return glfwCreateWindow(static_cast<int>(config.width), static_cast<int>(config.height), config.title, nullptr, nullptr);
				}
			}

			WindowModule* convertGLFWWindowToModule(GLFWwindow* win) {
				if (win == nullptr) {
					return nullptr;
				}

				WindowModule* windowModule = static_cast<WindowModule*>(glfwGetWindowUserPointer(win));
				if (windowModule == nullptr) {
					return nullptr;
				}

				return windowModule;
			}

			///GLFW callback functions

			void onGLFWError(int id, const CString desc) {
				if (id == GLFW_NOT_INITIALIZED) {
					MACE__THROW(InvalidState, "Windowing manager was not initialized/initialization was invalid: NOT_INITIALIZED: " + std::string(desc));
				} else if (id == GLFW_NO_CURRENT_CONTEXT) {
					MACE__THROW(gfx::NoRendererContext, "No Renderer context in this thread: NO_CURRENT_CONTEXT: " + std::string(desc));
				} else if (id == GLFW_FORMAT_UNAVAILABLE) {
					MACE__THROW(gfx::BadFormat, "The system does not support the required format: FORMAT_UNAVAIBLE: " + std::string(desc));
				} else if (id == GLFW_NO_WINDOW_CONTEXT) {
					MACE__THROW(gfx::NoRendererContext, "No window in this thread: NO_WINDOW_CONTEXT: " + std::string(desc));
				} else if (id == GLFW_API_UNAVAILABLE) {
					MACE__THROW(gfx::UnsupportedRenderer, "Context unavailable on this system: API_UNAVAILABLE" + std::string(desc));
				} else if (id == GLFW_VERSION_UNAVAILABLE) {
					MACE__THROW(gfx::UnsupportedRenderer, "Context unavailable on this system: VERSION_UNAVAILABLE: " + std::string(desc));
				} else if (id == GLFW_PLATFORM_ERROR) {
					MACE__THROW(System, "An error occured in the window platform: PLATFORM_ERROR: " + std::string(desc));
				} else if (id == GLFW_OUT_OF_MEMORY) {
					MACE__THROW(OutOfMemory, "Out of memory: OUT_OF_MEMORY: " + std::string(desc));
				} else {
					MACE__THROW(gfx::Window, desc);
				}
			}

			void onWindowClose(GLFWwindow* window) {
				WindowModule* mod = convertGLFWWindowToModule(window);
				mod->makeDirty();

				const WindowModule::LaunchConfig& config = mod->getLaunchConfig();
				config.onClose(*mod);

				if (config.terminateOnClose) {
					mod->getInstance()->requestStop();
				}
			}

			void onWindowKeyButton(GLFWwindow*, int key, int, int action, int mods) {
				Byte actions = 0x00;
				if (action == GLFW_PRESS) {
					actions |= Input::PRESSED;
				}
				if (action == GLFW_REPEAT) {
					actions |= Input::REPEATED;
				}
				if (action == GLFW_RELEASE) {
					actions |= Input::RELEASED;
				}
				if (mods & GLFW_MOD_SHIFT) {
					actions |= Input::MODIFIER_SHIFT;
				}
				if (mods & GLFW_MOD_CONTROL) {
					actions |= Input::MODIFIER_CONTROL;
				}
				if (mods & GLFW_MOD_ALT) {
					actions |= Input::MODIFIER_ALT;
				}
				if (mods & GLFW_MOD_SUPER) {
					actions |= Input::MODIFIER_SUPER;
				}

				pushKeyEvent(static_cast<Input::KeyCode> (key), actions);
			}

			void onWindowMouseButton(GLFWwindow*, int button, int action, int mods) {
				Byte actions = 0x00;
				if (action == GLFW_PRESS) {
					actions |= Input::PRESSED;
				}
				if (action == GLFW_REPEAT) {
					actions |= Input::REPEATED;
				}
				if (action == GLFW_RELEASE) {
					actions |= Input::RELEASED;
				}
				if (mods & GLFW_MOD_SHIFT) {
					actions |= Input::MODIFIER_SHIFT;
				}
				if (mods & GLFW_MOD_CONTROL) {
					actions |= Input::MODIFIER_CONTROL;
				}
				if (mods & GLFW_MOD_ALT) {
					actions |= Input::MODIFIER_ALT;
				}
				if (mods & GLFW_MOD_SUPER) {
					actions |= Input::MODIFIER_SUPER;
				}

				//in case that we dont have it mapped the same way that GLFW does, we add MOUSE_FIRST which is the offset to the mouse bindings.
				pushKeyEvent(static_cast<Input::KeyCode>(button) + Input::MOUSE_FIRST, actions);
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
				win->makeChildrenDirty();
			}

			void onWindowDamaged(GLFWwindow* window) {
				convertGLFWWindowToModule(window)->makeDirty();
			}

			VideoMode createVideoMode(const GLFWvidmode* mode) {
				VideoMode out{};
				out.width = mode->width;
				out.height = mode->height;
				out.channelBits = Color(mode->redBits, mode->greenBits, mode->blueBits);
				out.refreshRate = mode->refreshRate;
				return out;
			}
		}//anon namespace

		WindowModule::WindowModule(const LaunchConfig& c) : config(c), properties(0), window(nullptr) {}

		void WindowModule::create() {
			glfwSetErrorCallback(&onGLFWError);

			if (!glfwInit()) {
				MACE__THROW(InitializationFailed, "GLFW failed to initialize!");
			}

			os::clearError(__LINE__, __FILE__);//glfwInit sometimes sets LastError to non-zero. we can ignore it

			//just in case someone changed some before creating the window module
			glfwDefaultWindowHints();

			ComponentPtr<GraphicsContextComponent> context;

			switch (config.contextType) {
			case LaunchConfig::ContextType::AUTOMATIC:
				MACE_FALLTHROUGH;
			case LaunchConfig::ContextType::BEST_OGL:
				MACE_FALLTHROUGH;
			case LaunchConfig::ContextType::OGL33:
				MACE_FALLTHROUGH;
			default:
				context = ComponentPtr<gfx::GraphicsContextComponent>(new MACE__INTERNAL_NS::ogl33::OGL33Context(this));

				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

				glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

				glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

				glfwWindowHint(GLFW_STENCIL_BITS, 8);
				glfwWindowHint(GLFW_DEPTH_BITS, GLFW_DONT_CARE);
#ifdef MACE_DEBUG
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#else
				glfwWindowHint(GLFW_CONTEXT_NO_ERROR, true);
#endif
				break;
			}

			MACE_ASSERT(context != nullptr, "Internal Error: GraphicsContextComponent is nullptr");

			addComponent<GraphicsContextComponent>(context);

			glfwWindowHint(GLFW_RESIZABLE, config.resizable);
			glfwWindowHint(GLFW_DECORATED, config.decorated);

			window = createWindow(config);

			if (config.contextType == LaunchConfig::ContextType::BEST_OGL || config.contextType == LaunchConfig::ContextType::AUTOMATIC) {
				int versionMajor = 3;

				//this checks every available context until 1.0. the window is hidden so it won't cause spazzing
				for (int versionMinor = 3; versionMinor >= 0 && !window; --versionMinor) {
					std::cout << os::consoleColor(os::ConsoleColor::YELLOW) << "Trying OpenGL ";
					std::cout << os::consoleColor(os::ConsoleColor::LIGHT_YELLOW) << versionMajor << "." << versionMinor << std::endl << os::consoleColor();
					glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versionMajor);
					glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versionMinor);

					glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

					window = createWindow(config);
					if (versionMinor == 0 && versionMajor > 1 && !window) {
						glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, false);
						versionMinor = 3;
						--versionMajor;
					}
				}
			}

			if (window == nullptr) {
				MACE__THROW(InitializationFailed, "OpenGL context was unable to be created. This graphics card may not be supported or the graphics drivers are installed incorrectly");
			}
		}//create 

		void WindowModule::configureThread() {
			glfwMakeContextCurrent(window);

			if (config.vsync) {
				glfwSwapInterval(1);
			} else {
				if (glfwExtensionSupported("WGL_EXT_swap_control_tear") || glfwExtensionSupported("GLX_EXT_swap_control_tear")) {
					glfwSwapInterval(-1);
				} else {
					glfwSwapInterval(0);
				}
			}

			glfwSetWindowUserPointer(window, this);

			glfwSetWindowCloseCallback(window, &onWindowClose);
			glfwSetKeyCallback(window, &onWindowKeyButton);
			glfwSetMouseButtonCallback(window, &onWindowMouseButton);
			glfwSetCursorPosCallback(window, &onWindowCursorPosition);
			glfwSetScrollCallback(window, &onWindowScrollWheel);

			glfwSetFramebufferSizeCallback(window, &onWindowFramebufferResized);
			glfwSetWindowRefreshCallback(window, &onWindowDamaged);

			//int width = 0, height = 0;

			//glfwGetFramebufferSize(window, &width, &height);

			//if (width != config.width || height != config.height) {
			//	context->getRenderer()->resize(this, width, height);
			//}
		}

		const WindowModule::LaunchConfig& WindowModule::getLaunchConfig() const {
			return config;
		}

		void WindowModule::setTitle(const std::string& newTitle) {
			if (!isInit()) {
				MACE__THROW(InvalidState, "WindowModule not initialized! Must call MACE::init() first!");
			}

			glfwSetWindowTitle(window, newTitle.c_str());
		}

		void WindowModule::threadCallback() {
			try {
				os::clearError(__LINE__, __FILE__);

				//typedefs for chrono for readibility purposes
				using Clock = std::chrono::steady_clock;
				using TimeStamp = std::chrono::time_point<Clock>;
				using Duration = std::chrono::microseconds;

				//mutex for this function.
				std::mutex mutex;

				//now is set to be now() every loop, and the delta is calculated from now and last frame.
				TimeStamp now = Clock::now();
				//each time the frame is swapped, lastFrame is updated with the new time
				TimeStamp lastFrame = Clock::now();

				//this stores how many milliseconds it takes for the frame to swap.
				Duration windowDelay = Duration::zero();

				try {
					const std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex

					configureThread();

					Entity::init();

					getContext()->getRenderer()->resize(this, config.width, config.height);

					config.onCreate(*this);

					if (config.fps != 0) {
						windowDelay = Duration(std::chrono::seconds(1)) / static_cast<long long>(config.fps);
					}

					os::clearError(__LINE__, __FILE__);
				} catch (const std::exception& e) {
					handleError(e, instance);
				} catch (...) {
					MACE__THROW(Unknown, "An unknown error has occured trying to initalize MACE");
				}

				auto renderer = getContext()->getRenderer();

				//this is the main rendering loop.
				//we loop infinitely until break is called. break is called when an exception is thrown or MACE::isRunning is false
				for (;;) {//( ;_;)
					try {
						//thread doesn't own window, so we have to lock the mutex
						const std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex

						if (getProperty(Entity::DIRTY)) {
							renderer->setUp(this);
							setProperty(Entity::DIRTY, false);
							Entity::clean();
							Entity::render();
							renderer->tearDown(this);
						}

						renderer->checkInput(this);

						if (!instance->isRunning()) {
							//pressing the X button on a window sends a SIGABRT which throws an error later
							os::clearError(__LINE__, __FILE__);
							break; // while (!MACE::isRunning) would require a lock on destroyed or have it be an atomic varible, both of which are undesirable. while we already have a lock, set a stack variable to false.that way, we only read it, and we dont need to always lock it
						}
					} catch (const std::exception& e) {
						handleError(e, instance);
						break;
					} catch (...) {
						MACE__THROW(Unknown, "An unknown error occured trying to render a frame");
					}

					if (windowDelay != Duration::zero()) {
						now = Clock::now();

						std::this_thread::sleep_for(windowDelay - (now - lastFrame));

						lastFrame = Clock::now();
					}
				}

				os::checkError(__LINE__, __FILE__, "A system error occurred during the window loop");

				try {
					const std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex

					Entity::destroy();

					//the window will be destroyed on the main thread, need to detach this one
					glfwMakeContextCurrent(nullptr);

					os::checkError(__LINE__, __FILE__, "A system error occurred destroying the window");
				} catch (const std::exception& e) {
					handleError(e, instance);
				} catch (...) {
					MACE__THROW(Unknown, "An unknown error occured trying to destroy the rendering thread");
				}

				os::checkError(__LINE__, __FILE__, "A system error occured while running MACE");
			} catch (const std::exception& e) {
				handleError(e, instance);
			} catch (...) {
				handleError(MACE__GET_ERROR_NAME(Unknown) ("An unknown error occured while running MACE", MACE_STRINGIFY_DEFINITION(__LINE__), __FILE__), instance);
			}
		}//threadCallback

		void WindowModule::init() {
			//GLFW needs to be created from main thread
			//we create a window in the main thread, then switch its context to the render thread
			create();

			os::clearError(__LINE__, __FILE__);//sometimes an error comes from GLFW that we can ignore

			//release context on this thread, it wll be owned by the seperate rendering thread
			glfwMakeContextCurrent(nullptr);

			windowThread = std::thread(&WindowModule::threadCallback, this);

			os::checkError(__LINE__, __FILE__, "A system error occured while trying to init the WindowModule");
		}

		void WindowModule::update() {
			std::mutex mutex;
			const std::unique_lock<std::mutex> guard(mutex);

			glfwPollEvents();

			Entity::update();
		}//update

		void WindowModule::destroy() {
			{
				std::mutex mutex;
				const std::unique_lock<std::mutex> guard(mutex);
				setProperty(gfx::Entity::DEAD, true);
			}

			windowThread.join();

			os::checkError(__LINE__, __FILE__, "A system error occured while trying to destroy the WindowModule");

			glfwDestroyWindow(window);
			glfwMakeContextCurrent(nullptr);

			glfwTerminate();
			os::clearError(__LINE__, __FILE__);//https://github.com/glfw/glfw/issues/1053
		}//destroy

		std::string WindowModule::getName() const {
			return "MACE/Window#" + std::string(config.title);
		}//getName()

		bool WindowModule::isDestroyed() const {
			return properties & WindowModule::DESTROYED;
		}

		Vector<Pixels, 2> WindowModule::getFramebufferSize() const {
			if (!isInit()) {
				MACE__THROW(InvalidState, "WindowModule not initialized! Must call MACE::init() first!");
			}

			Vector<int, 2> out = {};

			int width, height;

			glfwGetFramebufferSize(window, &width, &height);

			return {static_cast<Pixels>(width), static_cast<Pixels>(height)};
		}

		Vector<Pixels, 2> WindowModule::getWindowSize() const {
			if (!isInit()) {
				MACE__THROW(InvalidState, "WindowModule not initialized! Must call MACE::init() first!");
			}

			Vector<int, 2> out = {};

			int width, height;

			glfwGetWindowSize(window, &width, &height);

			return {static_cast<Pixels>(width), static_cast<Pixels>(height)};
		}

		Vector<float, 2> WindowModule::getContentScale() const {
			if (!getProperty(gfx::Entity::INIT)) {
				MACE__THROW(InvalidState, "WindowModule not initialized! Must call MACE::init() first!");
			}

			Vector<float, 2> out = {};

			glfwGetWindowContentScale(window, &out[0], &out[1]);

			return out;
		}

		ComponentPtr<GraphicsContextComponent> WindowModule::getContext() {
			return getComponent<GraphicsContextComponent>();
		}

		const ComponentPtr<GraphicsContextComponent> WindowModule::getContext() const {
			return getComponent<GraphicsContextComponent>();
		}

		Monitor WindowModule::getMonitor() {
			GLFWmonitor* const mon = glfwGetWindowMonitor(window);
			if (mon != nullptr) {
				return Monitor(mon);
			}
			return Monitor(glfwGetPrimaryMonitor());
		}

		void WindowModule::onInit() {}

		void WindowModule::onUpdate() {}

		void WindowModule::onRender() {}

		void WindowModule::onDestroy() {}

		void WindowModule::clean() {
			setProperty(Entity::DIRTY, false);
		}//clean()

		namespace Input {
			const Byte& getKey(const KeyCode key) {
				return keys[key];
			}

			bool isKeyDown(const KeyCode key) {
				return keys[key] & Input::PRESSED || keys[key] & Input::REPEATED;
			}

			bool isKeyRepeated(const KeyCode key) {
				return keys[key] & Input::REPEATED;
			}

			bool isKeyReleased(const KeyCode key) {
				return keys[key] & Input::RELEASED;
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

		WindowModule::LaunchConfig::LaunchConfig(const Pixels w, const Pixels h, CString t) : title(t), width(w), height(h) {}

		bool WindowModule::LaunchConfig::operator==(const LaunchConfig& other) const {
			return title == other.title && width == other.width && height == other.height
				&& fps == other.fps && contextType == other.contextType
				&& terminateOnClose == other.terminateOnClose
				&& decorated == other.decorated && fullscreen == other.fullscreen
				&& resizable == other.resizable && vsync == other.vsync;
		}

		bool WindowModule::LaunchConfig::operator!=(const LaunchConfig& other) const {
			return !operator==(other);
		}

		Monitor::Monitor(GLFWmonitor* const mon) : monitor(mon) {}

		WindowModule* getCurrentWindow() {
			WindowModule* win = getCurrentWindowOrNull();
			if (win == nullptr) {
				MACE__THROW(gfx::NoRendererContext, "No Renderer found in this thread");
			}

			return win;
		}

		WindowModule* getCurrentWindowOrNull() {
			return convertGLFWWindowToModule(glfwGetCurrentContext());
		}

		bool hasWindow() {
			return getCurrentWindowOrNull() != nullptr;
		}

		Monitor getPrimaryMonitor() {
			return Monitor(glfwGetPrimaryMonitor());
		}

		Vector<unsigned int, 2> Monitor::getSizeMM() const {
			int width, height;

			glfwGetMonitorPhysicalSize(monitor, &width, &height);

			return {static_cast<unsigned int>(width), static_cast<unsigned int>(height)};
		}

		Vector<unsigned int, 2> Monitor::getSizeInches() const {
			const Vector<unsigned int, 2> mm = getSizeMM();
			return {static_cast<unsigned int>(static_cast<float>(mm[0]) / 25.4f), static_cast<unsigned int>(static_cast<float>(mm[1]) / 25.4f)};
		}

		Vector<unsigned int, 2> Monitor::getDPI() const {
			return getResolution() / getSizeInches();
		}

		Vector<float, 2> Monitor::getContentScale() const {
			Vector<float, 2> out = {};

			glfwGetMonitorContentScale(monitor, &out[0], &out[1]);

			return out;
		}

		CString Monitor::getName() const {
			return glfwGetMonitorName(monitor);
		}

		Vector<int, 4> Monitor::getWorkArea() const {
			Vector<int, 4> out = {};

			glfwGetMonitorWorkarea(monitor, &out[0], &out[1], &out[2], &out[3]);

			return out;
		}

		Vector<Pixels, 2> Monitor::getVirtualPosition() const {
			int x, y;

			glfwGetMonitorPos(monitor, &x, &y);

			return {static_cast<Pixels>(x), static_cast<Pixels>(y)};
		}

		std::vector<VideoMode> Monitor::getVideoModes() const {
			std::vector<VideoMode> out = std::vector<VideoMode>();

			int count = 0;
			const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count);
			for (int i = 0; i < count; ++i) {
				out.push_back(createVideoMode(&modes[i]));
			}
			return out;
		}

		VideoMode Monitor::getCurrentVideoMode()  const {
			return createVideoMode(glfwGetVideoMode(monitor));
		}

		Vector<Pixels, 2> Monitor::getResolution() const {
			VideoMode mode = getCurrentVideoMode();
			return {mode.width, mode.height};
		}
	}//os
}//mc
