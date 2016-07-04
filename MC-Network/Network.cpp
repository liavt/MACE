#include <MC-Network/Network.h>

namespace mc {
	void NetworkModule::init() {
		SDLNet_Init();
	}

	void NetworkModule::update() {

	}

	void NetworkModule::destroy() {
		SDLNet_Quit();
	}

	std::string NetworkModule::getName() const {
		return "MC-Network";
	}
}