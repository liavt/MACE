/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/MACE.h>

using namespace mc;

gfx::ProgressBar circleBar;
gfx::ProgressBar rectangleBar;

class TestComponent: public gfx::Component {
	void init(gfx::Entity* e) {};
	bool update(gfx::Entity* e) {
		if( gfx::Input::isKeyDown(gfx::Input::UP) ) {
			dynamic_cast<gfx::ProgressBar*>(e)->addProgress(1);
		} else if( gfx::Input::isKeyDown(gfx::Input::DOWN) ) {
			dynamic_cast<gfx::ProgressBar*>(e)->addProgress(-1);
		}
		return false;
	};
	void render(gfx::Entity* e) {};
	void destroy(gfx::Entity* e) {};
};

TestComponent r = TestComponent();

void create(gfx::WindowModule& win) {
	gfx::Texture circle = gfx::Texture::createFromFile(std::string(MACE_DEMO_ASSETS) + "/progressbar-circle.png", gfx::Enums::ImageFormat::LUMINANCE_ALPHA);

	gfx::GradientDesc gradDesc = gfx::GradientDesc(100, 100);
	gradDesc.botLeft = Colors::RED;
	gradDesc.botRight = Colors::GREEN;
	gradDesc.topLeft = Colors::BLUE;
	gradDesc.topRight = Colors::YELLOW;

	circleBar = gfx::ProgressBar(100, 255, 20);
	circleBar.setBackgroundTexture(gfx::Texture::createGradient(gradDesc));
	circleBar.setForegroundTexture(Colors::GREEN);
	circleBar.setSelectionTexture(circle);
	circleBar.setWidth(0.25f);
	circleBar.setHeight(0.25f);
	circleBar.setX(-0.5f);
	circleBar.addComponent(r);
	circleBar.setProperty(gfx::Entity::MAINTAIN_WIDTH, true);
	circleBar.setProperty(gfx::Entity::MAINTAIN_HEIGHT, true);

	circleBar.easeTo(250, 3500, gfx::EaseFunctions::BOUNCE_OUT);

	win.addChild(circleBar);

	rectangleBar = gfx::ProgressBar(0, 255, 50);
	rectangleBar.setBackgroundTexture(Colors::RED);
	rectangleBar.setForegroundTexture(gfx::Texture(gfx::Texture::getGradient(), Color(0.0f, 1.0f, 0.0f, 0.5f)));
	rectangleBar.setSelectionTexture(gfx::Texture::getGradient());
	rectangleBar.setWidth(0.1f);
	rectangleBar.setHeight(0.25f);
	rectangleBar.setX(0.5f);
	rectangleBar.addComponent(r);
	rectangleBar.setProperty(gfx::Entity::MAINTAIN_WIDTH, true);
	rectangleBar.setProperty(gfx::Entity::MAINTAIN_HEIGHT, true);

	rectangleBar.easeTo(200, 2000, gfx::EaseFunctions::ELASTIC_IN_OUT);

	win.addChild(rectangleBar);

	win.getContext()->getRenderer()->setRefreshColor(Colors::LIGHT_GRAY);
}

int main() {
	Instance instance = Instance();
	try {
		gfx::WindowModule::LaunchConfig config = gfx::WindowModule::LaunchConfig(500, 500, "Progress Bars Demo");
		config.onCreate = &create;
		config.resizable = true;

		gfx::WindowModule module = gfx::WindowModule(config);
		instance.addModule(module);

		os::ErrorModule errModule = os::ErrorModule();
		instance.addModule(errModule);

		instance.start();
	} catch( const std::exception& e ) {
		Error::handleError(e, instance);
		return -1;
	}
	return 0;
}
