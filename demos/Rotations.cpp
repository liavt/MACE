#include <MACE/MACE.h>
#include <random>
#include <iostream>

using namespace mc;

gfx::ColorAttachment star = gfx::ColorAttachment();

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
		if( os::Input::isKeyDown(os::Input::MOUSE_LEFT) ) {
			parent->rotate(0.0f, 0.0f, 0.01f);
		}
	}

	void destroy() override {}

	void clean() override {}
};

class RotationComponent: public gfx::Component {
	void init(gfx::Entity* e) {};
	bool update(gfx::Entity* e) {
		if( rotating ) {
			e->rotate(0.0f, 0.0f, 0.01f);
		}
		return false;
	};
	void render(gfx::Entity* e) {};
	void destroy(gfx::Entity* e) {};
};

TestComponent testComponent = TestComponent();


void create(os::WindowModule&) {
	srand((unsigned) time(0));

	const Size elementNum = 10;

	star = gfx::ColorAttachment(MACE_DEMO_ASSETS + std::string("star.png"));

	for( Index x = 0; x < elementNum; x++ ) {
		for( Index y = 0; y < elementNum; y++ ) {
			gfx::Image* entity = new gfx::Image();

			entity->setTexture(star);

			entity->setX((((x % (elementNum / 2)) *(1.0f / elementNum)) * 4) - (1.0f - (1.0f / elementNum) * 2));
			entity->setY((((y % (elementNum / 2)) * (1.0f / elementNum)) * 4) - (1.0f - (1.0f / elementNum) * 2));

			entity->setWidth((1.0f / elementNum) * 2);
			entity->setHeight((1.0f / elementNum) * 2);

			entity->addComponent(new gfx::PointerComponent(new TestComponent()));

			if( x >= elementNum / 2 ) {
				if( y >= elementNum / 2 ) {
					topRight.addChild(entity);
				} else {
					botRight.addChild(entity);
				}
			} else {
				if( y >= elementNum / 2 ) {
					topLeft.addChild(entity);
				} else {
					botLeft.addChild(entity);
				}
			}
		}
	}
}

int main() {
	try {
		std::cout << "Click on a star to rotate it individually! It is pixel perfect! Holding down space will rotate all the stars in big chunks. It will only swap frames when something changes for maximum performance.\n";
		std::cout << "Also try resizing the screen and watch how it reacts!\n";

		os::WindowModule::LaunchConfig config = os::WindowModule::LaunchConfig(500, 500, "Rotations Demo");
		config.onCreate = &create;
		os::WindowModule module = os::WindowModule(config);

		MACE::addModule(module);

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

		botLeft.addComponent(new gfx::PointerComponent(new RotationComponent()));
		botRight.addComponent(new gfx::PointerComponent(new RotationComponent()));
		topLeft.addComponent(new gfx::PointerComponent(new RotationComponent()));
		topRight.addComponent(new gfx::PointerComponent(new RotationComponent()));

		module.addChild(botLeft);
		module.addChild(botRight);
		module.addChild(topLeft);
		module.addChild(topRight);

		gfx::FPSComponent f = gfx::FPSComponent();
		f.setTickCallback([] (gfx::FPSComponent* com, gfx::Entity*) {
			std::cout << "UPS: " << com->getUpdatesPerSecond() << " FPS: " << com->getFramesPerSecond() << " Frame Time: " << float(1000.0f) / com->getFramesPerSecond() << std::endl;
		});
		module.addComponent(f);

		os::SignalModule sigModule = os::SignalModule();
		MACE::addModule(sigModule);

		mc::MACE::init();

		while( mc::MACE::isRunning() ) {
			mc::MACE::update();

			if( os::Input::isKeyDown(os::Input::SPACE) || os::Input::isKeyRepeated(os::Input::SPACE) ) {
				rotating = true;
			} else {
				rotating = false;
			}

			mc::os::wait(33);
		}

		mc::MACE::destroy();
	} catch( const std::exception& e ) {
		Error::handleError(e);
		return -1;
	}
	return 0;
}