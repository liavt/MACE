#include <MACE/MACE.h>
#include <random>
#include <ctime>
#include <iostream>

using namespace mc;

class TestComponent: public gfx::Component {

	void init(gfx::Entity* en) override {
		en->setProperty(gfx::Entity::MAINTAIN_X, true);
		en->setProperty(gfx::Entity::MAINTAIN_Y, true);
		en->setProperty(gfx::Entity::MAINTAIN_WIDTH, true);
		en->setProperty(gfx::Entity::MAINTAIN_HEIGHT, true);
	}

	bool update(gfx::Entity* en) override {
		return false;
	}

	void render(gfx::Entity* en) override {}

	void hover(gfx::Entity* en) override {
		if( os::Input::isKeyDown(os::Input::MOUSE_LEFT) ) {
			en->makeDirty();
		}
	}

	void destroy(gfx::Entity* en) override {}

	void clean(gfx::Entity* en) override {
		dynamic_cast<gfx::Image*>(en)->getTexture().setPaint(Color((rand() % 10) / 10.0f, (rand() % 10) / 10.0f, (rand() % 10) / 10.0f, 0.5f));
	}
};

TestComponent component = TestComponent();
gfx::Group group = gfx::Group();

gfx::Image left;
gfx::Image leftBot;
gfx::Image rightTop;
gfx::Image rightBot;

void create() {
	srand((unsigned) time(0));

	const Size elementNum = 10;

	left = gfx::Image(gfx::ColorAttachment(Colors::GREEN));
	leftBot = gfx::Image(gfx::ColorAttachment(Colors::GRAY));
	rightTop = gfx::Image(gfx::ColorAttachment(Colors::YELLOW));
	rightBot = gfx::Image(gfx::ColorAttachment(Colors::ORANGE));

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

int main() {
	try {

		os::WindowModule module = os::WindowModule(600, 500, "Cleaning Demo");

		module.setFPS(30);
		module.setVSync(false);
		module.setResizable(true);

		MACE::addModule(module);

		module.addChild(group);

		gfx::FPSComponent f = gfx::FPSComponent();
		f.setTickCallback([] (gfx::FPSComponent* com, gfx::Entity*) {
			std::cout << "UPS: " << com->getUpdatesPerSecond() << " FPS: " << com->getFramesPerSecond() << " Frame Time: " << float(1000.0f) / com->getFramesPerSecond() << std::endl;
		});
		module.addComponent(f);

		module.setCreationCallback(&create);

		mc::MACE::init();

		while( mc::MACE::isRunning() ) {
			mc::MACE::update();

			std::this_thread::sleep_for(std::chrono::milliseconds(33));
		}

		mc::MACE::destroy();



	} catch( const std::exception& e ) {
		Error::handleError(e);
		return -1;
	}
	return 0;
}
