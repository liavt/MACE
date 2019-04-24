/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/MACE.h>

using namespace mc;

gfx::Text topLeft, center, topRight, botLeft, botRight;

void create(gfx::WindowModule& win) {
	center = gfx::Text("FPS: 000 UPS: 000", gfx::Fonts::CODE);

	topLeft = gfx::Text(L"Top😎 left!", gfx::Fonts::SERIF);
	topLeft.setTexture(Colors::WHITE);
	topLeft.setHorizontalAlign(gfx::HorizontalAlign::LEFT);
	topLeft.setVerticalAlign(gfx::VerticalAlign::TOP);

	topRight = gfx::Text("Top right.", gfx::Fonts::SANS);
	topRight.setTexture(Colors::LIGHT_BLUE);
	topRight.setHorizontalAlign(gfx::HorizontalAlign::RIGHT);
	topRight.setVerticalAlign(gfx::VerticalAlign::TOP);

	botLeft = gfx::Text("bOtToM lEfT?", gfx::Font(gfx::Fonts::SERIF, 16));
	botLeft.setTexture(Colors::RED);
	botLeft.setHorizontalAlign(gfx::HorizontalAlign::LEFT);
	botLeft.setVerticalAlign(gfx::VerticalAlign::BOTTOM);

	gfx::Font font = gfx::Font::loadFont(MACE_DEMO_ASSETS + std::string("/arial.ttf"));
	font.setSize(24);

	botRight = gfx::Text(L"B0ttom ®1ght", font);
	botRight.setTexture(Colors::YELLOW);
	botRight.setHorizontalAlign(gfx::HorizontalAlign::RIGHT);
	botRight.setVerticalAlign(gfx::VerticalAlign::BOTTOM);

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

		instance.init();
		while (instance.isRunning()) {
			instance.update();

			module.makeDirty();

			mc::os::wait(33);
		}
		instance.destroy();
	} catch (const std::exception& e) {
		Error::handleError(e, instance);
		return -1;
	}
	return 0;
}
