#pragma once
#include <MC-System/System.h>
#include <SDL/SDL.h>
#include <memory>

namespace mc {
	class WindowModule;

	class Window {
		friend class WindowModule;

	protected:
		SDL_Window* m_window;
		int m_originalWidth;
		int m_originalHeight;
		char* m_title;
		
		virtual void create();
		virtual void update();
		virtual void destroy();


	public:
		Window(int width, int height, const char* title);
		bool isOpen();
		SDL_Window* getSDLWindow();
	};

	class WindowModule : public Module {
	public:
		WindowModule(Window* w);

		void init();
		void update();
		void destroy();
		std::string getName() const;
	private:
		Window* window;
	};
}