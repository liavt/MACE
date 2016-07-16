#pragma once
#include <MC-System/System.h>
#include <SDL/SDL_net.h>

namespace mc{
	/**
	Namespace with classes relating to networking
	*/
	namespace net
	{
		class NetworkModule : public Module {
		public:
			void init();
			void tick();
			void destroy();
			std::string getName() const;
		};
	}
}