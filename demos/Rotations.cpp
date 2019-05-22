/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/MACE.h>
#include <random>
#include <iostream>

using namespace mc;

bool rotating = false;

gfx::Group botLeft, botRight, topLeft, topRight;

class TestComponent: public gfx::Component {

	void init() override {
		dynamic_cast<gfx::Image*>(parent)->getTexture().setHue(Color((rand() % 10) / 10.0f, (rand() % 10) / 10.0f, (rand() % 10) / 10.0f, 0.5f));
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

	void clean(gfx::Metrics&) override {}
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
	
	const gfx::Texture star = gfx::Texture::createFromFile(MACE_DEMO_ASSETS + std::string("star.png"), gfx::ImageFormat::DONT_CARE);
	
	for (Index x = 0; x < elementNum; x++) {
		for (Index y = 0; y < elementNum; y++) {
			gfx::Image* entity = new gfx::Image();

			entity->setTexture(star);

			entity->setX((((x % (elementNum / 2)) * (1.0f / elementNum)) * 4) - (1.0f - (1.0f / elementNum) * 2));
			entity->setY((((y % (elementNum / 2)) * (1.0f / elementNum)) * 4) - (1.0f - (1.0f / elementNum) * 2));

			entity->setWidth((1.0f / elementNum) * 2);
			entity->setHeight((1.0f / elementNum) * 2);

			entity->getPainter().setOpacity(0.5f);

			entity->addComponent(std::shared_ptr<TestComponent>(new TestComponent()));

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

		botLeft.addComponent(std::shared_ptr<RotationComponent>(new RotationComponent()));
		botRight.addComponent(std::shared_ptr<RotationComponent>(new RotationComponent()));
		topLeft.addComponent(std::shared_ptr<RotationComponent>(new RotationComponent()));
		topRight.addComponent(std::shared_ptr<RotationComponent>(new RotationComponent()));

		module.addChild(botLeft);
		module.addChild(botRight);
		module.addChild(topLeft);
		module.addChild(topRight);

		gfx::FPSComponent f = gfx::FPSComponent();
		f.setTickCallback([](gfx::FPSComponent* com, gfx::Entity*) {
			std::cout << "UPS: " << com->getUpdatesPerSecond() << " FPS: " << com->getFramesPerSecond() << " Frame Time: " << float(1000.0f) / com->getFramesPerSecond() << std::endl;
		});
		module.addComponent(f);

		os::ErrorModule errModule = os::ErrorModule();
		instance.addModule(errModule);

		instance.init();
		while (instance.isRunning()) {
			instance.update();

			if (gfx::Input::isKeyDown(gfx::Input::SPACE) || gfx::Input::isKeyRepeated(gfx::Input::SPACE)) {
				rotating = true;
			} else {
				rotating = false;
			}

			mc::os::wait(33);
		}
		instance.destroy();
	} catch (const std::exception& e) {
		Error::handleError(e, instance);
		return -1;
	} catch (...) {
		std::cerr << "An unknown exception occured";
		return -1;
	}

	return 0;
}
