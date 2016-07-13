#include <MC-Graphics/Graphics.h>
#include <iostream>

namespace mc {
	GraphicsModule::GraphicsModule() :EntityModule()
	{
	}
	void GraphicsModule::init() {
		std::cout << "Inited!" << std::endl;
		mc::System::assertModule("MC-Window");
	}

	void GraphicsModule::update() {
		std::cout << "Tickd!" << std::endl;
	}

	void GraphicsModule::destroy() {
		std::cout << "Destroyed!" << std::endl;
	}

	std::string GraphicsModule::getName() const{
		return "MC-Graphics";
	}
}