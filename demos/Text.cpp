#include <MACE/MACE.h>

#include <iostream>

using namespace mc;

gfx::Group group;

gfx::Text topLeft, center, topRight, botLeft, botRight;

void create() {
	gfx::Renderer::setRefreshColor(Colors::DARK_GREEN);

	gfx::Font font = gfx::Font::loadFont(MACE_DEMO_ASSETS + std::string("/consola.ttf"));
	font.setSize(48);

	center = gfx::Text(L"©enter text!", font);

	topLeft = gfx::Text("Top left!", font);
	topLeft.setTexture(Colors::WHITE);
	topLeft.setHorizontalAlign(gfx::HorizontalAlign::LEFT);
	topLeft.setVerticalAlign(gfx::VerticalAlign::TOP);

	topRight = gfx::Text("Top right.", font);
	topRight.setTexture(Colors::LIGHT_BLUE);
	topRight.setHorizontalAlign(gfx::HorizontalAlign::RIGHT);
	topRight.setVerticalAlign(gfx::VerticalAlign::TOP);

	botLeft = gfx::Text("bOtToM lEfT?", font);
	botLeft.setTexture(Colors::RED);
	botLeft.setHorizontalAlign(gfx::HorizontalAlign::LEFT);
	botLeft.setVerticalAlign(gfx::VerticalAlign::BOTTOM);

	botRight = gfx::Text(L"B0ttom ®1ght", font);
	botRight.setTexture(Colors::YELLOW);
	botRight.setHorizontalAlign(gfx::HorizontalAlign::RIGHT);
	botRight.setVerticalAlign(gfx::VerticalAlign::BOTTOM);

	group.addChild(center);
	group.addChild(topLeft);
	group.addChild(topRight);
	group.addChild(botLeft);
	group.addChild(botRight);
}

int main() {
	try {
		os::WindowModule module = os::WindowModule(600, 600, "Text Demo");
		module.setResizable(true);
		module.setFPS(30);

		module.addChild(group);

		MACE::addModule(module);

		module.setCreationCallback(&create);

		mc::MACE::init();

		while( mc::MACE::isRunning() ) {
			mc::MACE::update();

			std::this_thread::sleep_for(std::chrono::milliseconds(33));
		}

		mc::MACE::destroy();
	} catch( const std::exception& e ) {
		Exception::handleException(e);
		return -1;
	}
	return 0;
}
