#include <MC-Graphics/Graphics.h>
#include <iostream>

namespace mc {
	void GraphicsModule::init() {
		std::cout << "Inited!" << std::endl;
		mc::System::assertModule("MC-Window");
		mc::System::assertModule("MC-Entity");
	}

	void GraphicsModule::update() {
		std::cout << "Updated!" << std::endl;
	}

	void GraphicsModule::destroy() {
		std::cout << "Destroyed!" << std::endl;
	}

	std::string GraphicsModule::getName() const{
		return "MC-Graphics";
	}
}