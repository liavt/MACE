#include <MACE/MACE.h>

#include <iostream>

using namespace mc;

gfx::Group group;

gfx::ProgressBar circleBar;
gfx::ProgressBar rectangleBar;

gfx::Letter img;

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
	gfx::ogl::Texture background = gfx::ogl::Texture(std::string(MACE_DEMO_ASSETS) + "/progressbar-circlebackground.png");
	gfx::ogl::Texture foreground = gfx::ogl::Texture(std::string(MACE_DEMO_ASSETS) + "/progressbar-foreground.png");
	gfx::ogl::Texture selection = gfx::ogl::Texture(std::string(MACE_DEMO_ASSETS) + "/progressbar-circle.png");

	circleBar = gfx::ProgressBar(0, 255, 20);
	circleBar.setBackgroundTexture(background);
	circleBar.setForegroundTexture(foreground);
	circleBar.setSelectionTexture(selection);
	circleBar.setWidth(0.25f);
	circleBar.setHeight(0.25f);
	circleBar.setX(-0.5f);
	circleBar.addComponent(r);

	circleBar.easeTo(250, 100, gfx::ProgressBar::EaseFunction::SQUARE_ROOT);

	group.addChild(circleBar);

	selection = gfx::ogl::Texture(std::string(MACE_DEMO_ASSETS) + "/progressbar-gradient.png");
	background = gfx::ogl::Texture(std::string(MACE_DEMO_ASSETS) + "/progressbar-background.png");

	rectangleBar = gfx::ProgressBar(0, 255, 100);
	rectangleBar.setBackgroundTexture(background);
	rectangleBar.setForegroundTexture(foreground);
	rectangleBar.setSelectionTexture(selection);
	rectangleBar.setWidth(0.1f);
	rectangleBar.setHeight(0.25f);
	rectangleBar.setX(0.5f);
	rectangleBar.addComponent(r);

	rectangleBar.easeTo(250, 100, gfx::ProgressBar::EaseFunction::SINUSOIDAL);

	group.addChild(rectangleBar);

	gfx::Font font = gfx::Font::loadFont(MACE_DEMO_ASSETS+std::string("/arial.ttf"));
	font.setSize(250);
	img = font.getCharacter(L'❤');
    img.setWidth(0.5f);
	img.setHeight(0.5f);
	group.addChild(img);
}

int main() {
	try {
		os::WindowModule module = os::WindowModule(500, 500, "Progress Bars Demo");

		module.setFPS(30);
		module.setVSync(false);

		module.addChild(group);

		System::addModule(module);

		module.setCreationCallback(&create);

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
