#pragma once
#include <MC-System/System.h>
#include <SDL/SDL_net.h>

namespace mc{
	class NetworkModule : public Module {
	public:
		void init();
		void update();
		void destroy();
		std::string getName() const;
	};
}