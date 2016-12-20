#include <MACE/MACE.h>
#include <random>
#include <ctime>
#include <iostream>

using namespace mc;

gfx::ogl::Texture star = gfx::ogl::Texture();

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

class TestEntity: public gfx::Entity2D {

	void onUpdate() override {
	};
	void onRender() override {
		std::cout << getTexture().getID();
		gfx::Renderer::queue(this);
	};
	void onInit() override {
	};
	void onDestroy() override {};

	void onClean() override {
		setPaint(Color((rand() % 10) / 10.0f, (rand() % 10) / 10.0f, (rand() % 10) / 10.0f, 0.5f));
	};
};

gfx::Group group = gfx::Group();

void initGL() {
	srand((unsigned) time(0));

	const Size elementNum = 10;

	star = gfx::ogl::Texture(MACE_DEMO_ASSETS + std::string("star.png"));

	for( Index x = 0; x < elementNum; x++ ) {
		for( Index y = 0; y < elementNum; y++ ) {
			TestEntity* entity = new TestEntity();

			entity->setTexture(star);

			entity->setX((((x % (elementNum / 2)) *(1.0f / elementNum) * 2)));
			entity->setY((((y % (elementNum / 2)) * (1.0f / elementNum) * 2)));

			entity->setWidth((1.0f / elementNum) * 2);
			entity->setHeight((1.0f / elementNum) * 2);

			entity->setProperty(gfx::Entity::STRETCH_X, false);
			entity->setProperty(gfx::Entity::STRETCH_Y, false);

			group.addChild(*entity);
		}
	}


}

int main(int argc, char* argv) {
	try {

		os::WindowModule module = os::WindowModule(500, 500, "Cleaning Demo");

		module.setFPS(30);
		module.setVSync(false);

		System::addModule(module);

		group.setWidth(1.0f);
		group.setHeight(1.0f);

		module.addChild(group);

		FPSEntity f = FPSEntity();
		module.addChild(f);

		gfx::CallbackEntity callback;
		callback.setInitCallback(&initGL);
		module.addChild(callback);

		mc::System::init();

		while( mc::System::isRunning() ) {
			mc::System::update();

			std::this_thread::sleep_for(std::chrono::milliseconds(33));
		}

		mc::System::destroy();



	} catch( const std::exception& e ) {
		Exception::handleException(e);
		return -1;
	}
	return 0;
}
