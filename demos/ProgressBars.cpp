#include <MACE/MACE.h>

using namespace mc;

gfx::Group group;

gfx::ProgressBar circleBar;

class TestComponent: public gfx::Component {
	void init(gfx::Entity* e) {};
	bool update(gfx::Entity* e) {
		if( os::Input::isKeyDown(os::Input::UP) ) {
			dynamic_cast<gfx::ProgressBar*>(e)->setProgress(++circleBar.getProgress());
		}else if( os::Input::isKeyDown(os::Input::DOWN) ) {
			dynamic_cast<gfx::ProgressBar*>(e)->setProgress(--circleBar.getProgress());
		}
		return false;
	};
	void render(gfx::Entity* e) {};
	void destroy(gfx::Entity* e) {};
};

TestComponent r = TestComponent();

void create() {
	gfx::ogl::Texture background = gfx::ogl::Texture(std::string(MACE_DEMO_ASSETS) + "/progressbar-background.png");
	gfx::ogl::Texture foreground = gfx::ogl::Texture(std::string(MACE_DEMO_ASSETS) + "/progressbar-foreground.png");
	gfx::ogl::Texture circle = gfx::ogl::Texture(std::string(MACE_DEMO_ASSETS)+"/progressbar-circle.png");

	circleBar = gfx::ProgressBar(0, 100, 20);
	circleBar.setBackgroundTexture(background);
	circleBar.setForegroundTexture(foreground);
	circleBar.setSelectionTexture(circle);
	circleBar.addComponent(r);

	group.addChild(circleBar);
}

int main() {
	try {
		os::WindowModule module = os::WindowModule(500, 500, "Rotations Demo");

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