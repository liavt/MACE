/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/MACE.h>

using namespace mc;

gfx::Text topLeft, center, topRight, botLeft, botRight;

void create(gfx::WindowModule& win) {
	center = gfx::Text("FPS: 000 UPS: 000", gfx::Font(gfx::Fonts::CODE, 10));

	topLeft = gfx::Text(L"Top😎 left!", gfx::Fonts::SERIF);
	topLeft.setTexture(Colors::WHITE);
	topLeft.addComponent(std::shared_ptr<gfx::Component>(new gfx::AlignmentComponent(gfx::VerticalAlign::TOP, gfx::HorizontalAlign::LEFT)));

	topRight = gfx::Text("Top right.", gfx::Fonts::SANS);
	topRight.setTexture(Colors::LIGHT_BLUE);
	topRight.addComponent(std::shared_ptr<gfx::Component>(new gfx::AlignmentComponent(gfx::VerticalAlign::TOP, gfx::HorizontalAlign::RIGHT)));

	botLeft = gfx::Text("bOtToM lEfT?", gfx::Font(gfx::Fonts::SERIF));
	botLeft.setTexture(Colors::RED);
	botLeft.addComponent(std::shared_ptr<gfx::Component>(new gfx::AlignmentComponent(gfx::VerticalAlign::BOTTOM, gfx::HorizontalAlign::LEFT)));

	gfx::Font font = gfx::Font::loadFont(MACE_DEMO_ASSETS + std::string("/arial.ttf"), 24);

	botRight = gfx::Text(L"B0ttom ®1ght", font);
	botRight.setTexture(Colors::YELLOW);
	botRight.addComponent(std::shared_ptr<gfx::Component>(new gfx::AlignmentComponent(gfx::VerticalAlign::BOTTOM, gfx::HorizontalAlign::RIGHT)));

	win.addChild(center);
	
	win.addChild(topLeft);
	win.addChild(topRight);
	win.addChild(botLeft);
	win.addChild(botRight);
	
	win.getContext()->getRenderer()->setRefreshColor(Colors::DARK_GREEN);
}

int main() {
	Instance instance = Instance();
	try {
		gfx::WindowModule::LaunchConfig config = gfx::WindowModule::LaunchConfig(600, 600, "Text Demo");
		config.onCreate = &create;
		config.resizable = true;

		gfx::WindowModule module = gfx::WindowModule(config);
		instance.addModule(module);

		os::ErrorModule errModule = os::ErrorModule();
		instance.addModule(errModule);

		gfx::FPSComponent f = gfx::FPSComponent();
		f.setTickCallback([](gfx::FPSComponent * com, gfx::Entity*) {
			center.setText("UPS: " + std::to_string(com->getUpdatesPerSecond()) + "\nFPS: " + std::to_string(com->getFramesPerSecond()) + "\nFrame Time: " + std::to_string(float(1000.0f) / com->getFramesPerSecond()));
		});
		module.addComponent(f);

		const Initializer init(instance);
		while (instance.isRunning()) {
			instance.update();

			module.makeDirty();

			mc::os::wait(33);
		}
	} catch (const std::exception& e) {
		Error::handleError(e, instance);
		return -1;
	}
	return 0;
}
