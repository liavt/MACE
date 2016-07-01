#include <MC-Graphics/Graphics.h>
#include <iostream>

void mc::GraphicsModule::init() {
	std::cout << "Inited!" << std::endl;
}

void mc::GraphicsModule::update() {
	std::cout << "Updated!" << std::endl;
}

void mc::GraphicsModule::destroy() {
	std::cout << "Destroyed!" << std::endl;
}

 mc::GraphicsModule::GraphicsModule() {
	 this->name = "Graphics";
}