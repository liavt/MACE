#include <MC-Graphics/Graphics.h>
#include <iostream>

namespace mc {
	namespace gfx{
		GraphicsModule::GraphicsModule() :EntityModule(), WindowModule()
		{
		}
		void GraphicsModule::init() {
			std::cout << "Inited!" << std::endl;
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
}