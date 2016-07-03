#pragma once
#include <MC-System/System.h>
#include <SDL/SDL.h>
#include <memory>
#include <MC-Window/Window.h>

namespace mc {
	class WindowModule : public Module {
	public:
		WindowModule();

		void init();
		void update();
		void destroy();
		std::string getName() const;
	};
}