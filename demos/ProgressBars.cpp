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

void create(os::WindowModule&) {
	gfx::Texture circle = gfx::Texture(std::string(MACE_DEMO_ASSETS) + "/progressbar-circle.png");

	circleBar = gfx::ProgressBar(100, 255, 20);
	circleBar.setBackgroundTexture(gfx::Texture(circle, Colors::RED));
	circleBar.setForegroundTexture(gfx::Texture(Colors::GREEN));
	circleBar.setSelectionTexture(circle);
	circleBar.setWidth(0.25f);
	circleBar.setHeight(0.25f);
	circleBar.setX(-0.5f);
	circleBar.addComponent(r);

	circleBar.easeTo(250, 100, gfx::EaseFunctions::BOUNCE_OUT);

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
		os::WindowModule::LaunchConfig config = os::WindowModule::LaunchConfig(500, 500, "Progress Bars Demo");
		config.onCreate = &create;
		config.resizable = true;
		os::WindowModule module = os::WindowModule(config);

		module.addChild(group);

		MACE::addModule(module);

		os::SignalModule sigModule = os::SignalModule();
		MACE::addModule(sigModule);

		MACE::start();
	} catch( const std::exception& e ) {
		Error::handleError(e);
		return -1;
	}
	return 0;
}
