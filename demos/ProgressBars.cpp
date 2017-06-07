#include <MACE/MACE.h>

#include <iostream>

using namespace mc;

gfx::Group group;

gfx::ProgressBar circleBar;
gfx::ProgressBar rectangleBar;

class TestComponent: public gfx::Component {
	void init(gfx::Entity* e) {};
	bool update(gfx::Entity* e) {
		if( os::Input::isKeyDown(os::Input::UP) ) {
			dynamic_cast<gfx::ProgressBar*>(e)->addProgress(1);
		} else if( os::Input::isKeyDown(os::Input::DOWN) ) {
			dynamic_cast<gfx::ProgressBar*>(e)->addProgress(-1);
		}
		return false;
	};
	void render(gfx::Entity* e) {};
	void destroy(gfx::Entity* e) {};
};

TestComponent r = TestComponent();

void create() {
	gfx::ColorAttachment circle = gfx::ColorAttachment(std::string(MACE_DEMO_ASSETS) + "/progressbar-circle.png");

	circleBar = gfx::ProgressBar(0, 255, 20);
	circleBar.setBackgroundTexture(gfx::ColorAttachment(circle, Colors::RED));
	circleBar.setForegroundTexture(gfx::ColorAttachment(Colors::GREEN));
	circleBar.setSelectionTexture(circle);
	circleBar.setWidth(0.25f);
	circleBar.setHeight(0.25f);
	circleBar.setX(-0.5f);
	circleBar.addComponent(r);

	circleBar.easeTo(250, 100, gfx::EaseFunctions::BOUNCE_IN_OUT);

	group.addChild(circleBar);

	rectangleBar = gfx::ProgressBar(0, 255, 50);
	rectangleBar.setBackgroundTexture(Colors::RED);
	rectangleBar.setForegroundTexture(Colors::GREEN);
	rectangleBar.setSelectionTexture(std::string(MACE_DEMO_ASSETS) + "/progressbar-gradient.png");
	rectangleBar.setWidth(0.1f);
	rectangleBar.setHeight(0.25f);
	rectangleBar.setX(0.5f);
	rectangleBar.addComponent(r);

	rectangleBar.easeTo(200, 100, gfx::EaseFunctions::ELASTIC_IN_OUT);

	group.addChild(rectangleBar);

	gfx::getRenderer()->setRefreshColor(Colors::LIGHT_GRAY);
}

int main() {
	try {
		os::WindowModule module = os::WindowModule(500, 500, "Progress Bars Demo");

		module.setFPS(30);
		module.setVSync(false);
		module.setResizable(true);

		module.addChild(group);

		MACE::addModule(module);

		module.setCreationCallback(&create);

		os::SignalModule sigModule = os::SignalModule();
		MACE::addModule(sigModule);

		MACE::start();
	} catch( const std::exception& e ) {
		Error::handleError(e);
		return -1;
	}
	return 0;
}
