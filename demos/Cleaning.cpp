#define MACE_DEMO_NAME Cleaning

#include <MACE/MACE.h>
#include <random>
#include <ctime>
#include <iostream>

namespace {
	using namespace mc;

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
			en->setProperty(gfx::Entity::STRETCH_X, true);
			en->setProperty(gfx::Entity::STRETCH_Y, true);
			dynamic_cast<gfx::GraphicsEntity*>(en)->setPaint(Colors::RED);
		}

		bool update(gfx::Entity* en) override {
			if( en->getProperty(gfx::Entity::HOVERED) && os::Input::isKeyDown(os::Input::MOUSE_LEFT) ) {
				en->makeDirty();
			}
			return false;
		}

		void destroy(gfx::Entity* en) override {}

		void clean(gfx::Entity* en) override {
			dynamic_cast<gfx::GraphicsEntity*>(en)->setPaint(Color((rand() % 10) / 10.0f, (rand() % 10) / 10.0f, (rand() % 10) / 10.0f, 0.5f));
		}
	};

	TestComponent component = TestComponent();
	gfx::Group group = gfx::Group();

	gfx::Image left;
	gfx::Image leftBot;
	gfx::Image rightTop;
	gfx::Image rightBot;

	void initGL() {
		srand((unsigned) time(0));

		const Size elementNum = 10;

		left = gfx::Image(Colors::RED);
		leftBot = gfx::Image(Colors::RED);
		rightTop = gfx::Image(Colors::RED);
		rightBot = gfx::Image(Colors::RED);


		left.setY(0.0f);
		left.setX(-0.5f);
		left.setHeight(0.9f);
		left.setWidth(0.45f);

		leftBot.setX(0.0f);
		leftBot.setY(-0.5f);
		leftBot.setWidth(0.7f);
		leftBot.setHeight(0.4f);

		left.addChild(leftBot);

		rightTop.setX(0.5f);
		rightTop.setY(0.5f);
		rightTop.setHeight(0.4f);
		rightTop.setWidth(0.4f);

		rightBot.setHeight(0.4f);
		rightBot.setWidth(0.4f);
		rightBot.setX(0.5f);
		rightBot.setY(-0.5f);

		left.addComponent(component);
		leftBot.addComponent(component);
		rightTop.addComponent(component);
		rightBot.addComponent(component);

		group.addChild(left);
		group.addChild(rightTop);
		group.addChild(rightBot);
	}
}//anon namespace

int MACE_DEMO_MAIN() {
	try {

		os::WindowModule module = os::WindowModule(600, 500, "Cleaning Demo");

		module.setFPS(30);
		module.setVSync(false);

		System::addModule(module);

		module.addChild(group);

		FPSEntity f = FPSEntity();
		module.addChild(f);

		gfx::CallbackEntity callback;
		callback.setInitCallback(&initGL);
		module.addChild(callback);

		mc::System::init();

		while( mc::System::isRunning() ) {
			mc::System::update();

#ifdef MACE_TEST_DEMOS
			mc::System::requestStop();
#else
			std::this_thread::sleep_for(std::chrono::milliseconds(33));
#endif		
		}

		mc::System::destroy();



	} catch( const std::exception& e ) {
		Exception::handleException(e);
		return -1;
	}
	return 0;
}