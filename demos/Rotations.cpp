
#include <MACE/MACE.h>
#include <random>
#include <ctime>
#include <iostream>

using namespace mc;

gfx::ogl::Texture star = gfx::ogl::Texture();

bool rotating = false;

gfx::Group botLeft, botRight, topLeft, topRight;

class FPSEntity: public gfx::Entity {
	Index nbFrames;
	time_t lastTime = time(0);

	void onUpdate() override {};
	void onRender() override {
		nbFrames++;
		if( time(0) - lastTime >= 1.0 ) {
			std::cout << "FPS: " << nbFrames << " Frame Time: " << float(1000.0f) / nbFrames << std::endl;
			nbFrames = 0;
			lastTime += 1;
		}
	};
	void onInit() override {};
	void onDestroy() override {};
};

class TestComponent: public gfx::Component {

	void init(gfx::Entity* en) override {
		dynamic_cast<gfx::Image*>(en)->setPaint(Color((rand() % 10) / 10.0f, (rand() % 10) / 10.0f, (rand() % 10) / 10.0f, 0.5f));
	}

	bool update(gfx::Entity* en) override {
		return false;
	}

	void hover(gfx::Entity* en) override {
		if( os::Input::isKeyDown(os::Input::MOUSE_LEFT) ) {
			en->rotate(0.0f, 0.0f, 0.01f);
		}
	}

	void destroy(gfx::Entity* en) override {}

	void clean(gfx::Entity* en) override {}
};

class RotationComponent: public gfx::Component {
	void init(gfx::Entity* e) {};
	bool update(gfx::Entity* e) {
		if( rotating ) {
			e->rotate(0.0f, 0.0f, 0.01f);
		}
		return false;
	};
	void destroy(gfx::Entity* e) {};
};

RotationComponent r = RotationComponent();
TestComponent testComponent = TestComponent();


void initGL() {



	srand((unsigned) time(0));

	const Size elementNum = 10;

	star = gfx::ogl::Texture(MACE_DEMO_ASSETS + std::string("star.png"));

	for( Index x = 0; x < elementNum; x++ ) {
		for( Index y = 0; y < elementNum; y++ ) {
			gfx::Image* entity = new gfx::Image();

			entity->setTexture(star);

			entity->setX((((x % (elementNum / 2)) *(1.0f / elementNum)) * 4) - (1.0f - (1.0f / elementNum) * 2));
			entity->setY((((y % (elementNum / 2)) * (1.0f / elementNum)) * 4) - (1.0f - (1.0f / elementNum) * 2));

			entity->setWidth((1.0f / elementNum) * 2);
			entity->setHeight((1.0f / elementNum) * 2);

			entity->setProperty(gfx::Entity::STRETCH_X, false);
			entity->setProperty(gfx::Entity::STRETCH_Y, false);

			entity->addComponent(testComponent);

			if( x >= elementNum / 2 ) {
				if( y >= elementNum / 2 ) {
					topRight.addChild(*entity);
				} else {
					botRight.addChild(*entity);
				}
			} else {
				if( y >= elementNum / 2 ) {
					topLeft.addChild(*entity);
				} else {
					botLeft.addChild(*entity);
				}
			}
		}
	}
}

int main() {
	try {
		std::cout << "Click on a star to rotate it individually! It is pixel perfect! Holding down space will rotate all the stars in big chunks. It will only swap frames when something changes for maximum performance.\n";
		std::cout << "Also try resizing the screen and watch how it reacts!\n";

		os::WindowModule module = os::WindowModule(500, 500, "Rotations Demo");

		module.setFPS(30);
		module.setVSync(false);

		System::addModule(module);

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

		botLeft.addComponent(r);
		botRight.addComponent(r);
		topLeft.addComponent(r);
		topRight.addComponent(r);

		module.addChild(botLeft);
		module.addChild(botRight);
		module.addChild(topLeft);
		module.addChild(topRight);

		FPSEntity f = FPSEntity();
		module.addChild(f);

		gfx::CallbackEntity callback;
		callback.setInitCallback(&initGL);
		module.addChild(callback);

		mc::System::init();

		while( mc::System::isRunning() ) {
			mc::System::update();

			if( os::Input::isKeyDown(os::Input::SPACE) || os::Input::isKeyRepeated(os::Input::SPACE) ) {
				rotating = true;
			} else {
				rotating = false;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(33));
		}

		mc::System::destroy();
	} catch( const std::exception& e ) {
		Exception::handleException(e);
		return -1;
	}
	return 0;
}