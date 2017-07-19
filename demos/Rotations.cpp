/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/MACE.h>
#include <random>
#include <iostream>

using namespace mc;

gfx::Texture star = gfx::Texture();

bool rotating = false;

gfx::Group botLeft, botRight, topLeft, topRight;

class TestComponent: public gfx::Component {

	void init() override {
		dynamic_cast<gfx::Image*>(parent)->getTexture().setPaint(Color((rand() % 10) / 10.0f, (rand() % 10) / 10.0f, (rand() % 10) / 10.0f, 0.5f));
	}

	bool update() override {
		return false;
	}

	void render() override {}

	void hover() override {
		if (gfx::Input::isKeyDown(gfx::Input::MOUSE_LEFT)) {
			parent->rotate(0.0f, 0.0f, 0.01f);
		}
	}

	void destroy() override {}

	void clean() override {}
};

class RotationComponent: public gfx::Component {
	void init() override {};
	bool update() override {
		if (rotating) {
			parent->rotate(0.0f, 0.0f, 0.01f);
		}
		return false;
	};
	void render() override {};
	void destroy() override {};
};

void create(gfx::WindowModule&) {
	srand((unsigned int)time(nullptr));

	const Size elementNum = 10;

	star = gfx::Texture::createFromFile(MACE_DEMO_ASSETS + std::string("star.png"), gfx::Enums::ImageFormat::GRAY_ALPHA);

	for (Index x = 0; x < elementNum; x++) {
		for (Index y = 0; y < elementNum; y++) {
			gfx::Image* entity = new gfx::Image();

			entity->setTexture(star);

			entity->setX((((x % (elementNum / 2)) * (1.0f / elementNum)) * 4) - (1.0f - (1.0f / elementNum) * 2));
			entity->setY((((y % (elementNum / 2)) * (1.0f / elementNum)) * 4) - (1.0f - (1.0f / elementNum) * 2));

			entity->setWidth((1.0f / elementNum) * 2);
			entity->setHeight((1.0f / elementNum) * 2);

			entity->setOpacity(0.5f);

			entity->addComponent(SmartPointer<gfx::Component>(new TestComponent(), true));

			if (x >= elementNum / 2) {
				if (y >= elementNum / 2) {
					topRight.addChild(entity);
				} else {
					botRight.addChild(entity);
				}
			} else {
				if (y >= elementNum / 2) {
					topLeft.addChild(entity);
				} else {
					botLeft.addChild(entity);
				}
			}
		}
	}
}

int main() {
	Instance instance = Instance();
	try {
		std::cout << "Click on a star to rotate it individually! It is pixel perfect! Holding down space will rotate all the stars in big chunks. It will only swap frames when something changes for maximum performance.\n";
		std::cout << "Also try resizing the screen and watch how it reacts!\n";

		gfx::WindowModule::LaunchConfig config = gfx::WindowModule::LaunchConfig(500, 500, "Rotations Demo");
		config.onCreate = &create;
		gfx::WindowModule module = gfx::WindowModule(config);

		instance.addModule(module);

		botLeft.setWidth(0.5f);
		botRight.setWidth(0.5f);
		topLeft.setWidth(0.5f);
		topRight.setWidth(0.5f);

		botLeft.setHeight(0.5f);
		botRight.setHeight(0.5f);
		topLeft.setHeight(0.5f);
		topRight.setHeight(0.5f);

		botRight.setX(0.5f);
		botRight.setY(-0.5f);

		topLeft.setX(-0.5f);
		topLeft.setY(0.5f);

		botLeft.setX(-0.5f);
		botLeft.setY(-0.5f);

		topRight.setX(0.5f);
		topRight.setY(0.5f);

		botLeft.addComponent(SmartPointer<gfx::Component>(new RotationComponent(), true));
		botRight.addComponent(SmartPointer<gfx::Component>(new RotationComponent(), true));
		topLeft.addComponent(SmartPointer<gfx::Component>(new RotationComponent(), true));
		topRight.addComponent(SmartPointer<gfx::Component>(new RotationComponent(), true));

		module.addChild(botLeft);
		module.addChild(botRight);
		module.addChild(topLeft);
		module.addChild(topRight);

		gfx::FPSComponent f = gfx::FPSComponent();
		f.setTickCallback([](gfx::FPSComponent* com, gfx::Entity*) {
			std::cout << "UPS: " << com->getUpdatesPerSecond() << " FPS: " << com->getFramesPerSecond() << " Frame Time: " << float(1000.0f) / com->getFramesPerSecond() << std::endl;
		});
		module.addComponent(f);

		os::SignalModule sigModule = os::SignalModule();
		instance.addModule(sigModule);

		mc::Initializer i(instance);
		while (instance.isRunning()) {
			instance.update();

			if (gfx::Input::isKeyDown(gfx::Input::SPACE) || gfx::Input::isKeyRepeated(gfx::Input::SPACE)) {
				rotating = true;
			} else {
				rotating = false;
			}

			mc::os::wait(33);
		}
	} catch (const std::exception& e) {
		Error::handleError(e, instance);
		return -1;
	}
	return 0;
}