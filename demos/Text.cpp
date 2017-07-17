/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/MACE.h>

using namespace mc;

gfx::Group group;

gfx::Text topLeft, center, topRight, botLeft, botRight;

void create(os::WindowModule& win) {
	win.getContext()->getRenderer()->setRefreshColor(Colors::DARK_GREEN);

	center = gfx::Text(L"©enter text!", gfx::Fonts::CODE);

	topLeft = gfx::Text("Top left!", gfx::Fonts::SERIF);
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
	font.setSize(48);

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
	Instance instance = Instance();
	try {
		os::WindowModule::LaunchConfig config = os::WindowModule::LaunchConfig(600, 600, "Text Demo");
		config.onCreate = &create;
		config.resizable = true;
		os::WindowModule module = os::WindowModule(config);
		module.addChild(group);

		instance.addModule(module);

		os::SignalModule sigModule = os::SignalModule();
		instance.addModule(sigModule);

		instance.start();
	} catch (const std::exception& e) {
		Error::handleError(e, instance);
		return -1;
	}
	return 0;
}
