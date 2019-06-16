/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/MACE.h>

using namespace mc;

gfx::SimpleSlider circleBar, rectangleBar;

class TestComponent: public gfx::Component {
	void init(gfx::Entity* e) {};
	bool update(gfx::Entity* e) {
		if( gfx::Input::isKeyDown(gfx::Input::UP) ) {
			dynamic_cast<gfx::Progressable*>(e)->addProgress(1);
		} else if( gfx::Input::isKeyDown(gfx::Input::DOWN) ) {
			dynamic_cast<gfx::Progressable*>(e)->addProgress(-1);
		}
		return false;
	};
	void render(gfx::Entity* e) {};
	void destroy(gfx::Entity* e) {};
};

TestComponent r = TestComponent();

void create(gfx::WindowModule& win) {
	circleBar = gfx::SimpleSlider(100, 255, 20);
	circleBar.setBackgroundTexture(gfx::Texture(gfx::Texture::createFromFile(std::string(MACE_DEMO_ASSETS) + "/star.png", gfx::ImageFormat::DONT_CARE), Color(Colors::DARK_GRAY, 0.5f)));
	circleBar.setForegroundTexture(Colors::GREEN);
	circleBar.setSelectionTexture(gfx::Texture::createFromFile(std::string(MACE_DEMO_ASSETS) + "/starGradient.png", gfx::ImageFormat::DONT_CARE));
	circleBar.setWidth(0.25f);
	circleBar.setHeight(0.25f);
	circleBar.setX(-0.5f);
	circleBar.addComponent(r);

	gfx::EaseSettings settings = gfx::EaseSettings();
	settings.ms = 3500;
	settings.ease = gfx::EaseFunctions::BOUNCE_OUT;
	circleBar.easeTo(250, settings);

	win.addChild(circleBar);

	rectangleBar = gfx::SimpleSlider(0, 255, 50);
	rectangleBar.setBackgroundTexture(Colors::RED);
	rectangleBar.setForegroundTexture(gfx::Texture(gfx::Texture::getGradient(), Color(0.0f, 1.0f, 0.0f, 0.5f)));
	rectangleBar.setSelectionTexture(gfx::Texture::getGradient());
	rectangleBar.setWidth(0.1f);
	rectangleBar.setHeight(0.25f);
	rectangleBar.setX(0.5f);
	rectangleBar.addComponent(r);

	settings = gfx::EaseSettings();
	settings.ms = 2000;
	settings.ease = gfx::EaseFunctions::ELASTIC_IN_OUT;
	rectangleBar.easeTo(200, settings);

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
		mc::handleError(e, instance);
		return -1;
	}
	return 0;
}
