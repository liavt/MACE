/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/MACE.h>

using namespace mc;

void create(gfx::WindowModule& win) {
	auto button1 = std::shared_ptr<gfx::UIButton>(new gfx::UIButton());
	button1->scale(0.2f, 0.2f);
	button1->addChild(std::shared_ptr<gfx::Text>(new gfx::Text("Hello")));
	win.addChild(button1);

	win.getContext()->getRenderer()->setRefreshColor(Colors::WHITE);
}

int main() {
	Instance instance = Instance();
	try {
		gfx::WindowModule::LaunchConfig config = gfx::WindowModule::LaunchConfig(600, 600, "Text Demo");
		config.onCreate = &create;
		config.resizable = true;

		gfx::WindowModule module = gfx::WindowModule(config);
		instance.addModule(module);

		os::ErrorModule errModule = os::ErrorModule();
		instance.addModule(errModule);

		instance.start();
	} catch (const std::exception& e) {
		mc::handleError(e, instance);
		return -1;
	}
	return 0;
}
