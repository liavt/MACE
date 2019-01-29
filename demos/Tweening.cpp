/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/MACE.h>

using namespace mc;

gfx::Image square;

void create(gfx::WindowModule& win) {
	square = gfx::Image(Colors::RED);

	square.setX(-0.5f);
	square.setY(0.5f);
	square.setWidth(0.1f);
	square.setHeight(0.1f);

	std::shared_ptr<gfx::ComponentQueue> queue = std::shared_ptr<gfx::ComponentQueue>(new gfx::ComponentQueue());

	TransformMatrix dest1 = TransformMatrix();
	dest1.scaler = { 0.2f, 0.2f, 0.0f };
	dest1.translation = { 0.5f, 0.5f, 0.0f };
	dest1.rotation = { 0.0f, 0.0f, 0.5f };

	gfx::EaseSettings settings = gfx::EaseSettings();
	settings.ms = 1000;
	settings.ease = gfx::EaseFunctions::BOUNCE_OUT;
	settings.repeats = 1;
	settings.reverseOnRepeat = false;
	queue->addComponent(std::shared_ptr<gfx::Component>(new gfx::TweenComponent(&square, dest1, settings)));

	TransformMatrix dest2 = TransformMatrix(dest1);
	dest2.scaler = { 0.5f, 0.5f, 0.0f };
	dest2.translation = { 0.5f, -0.5f, 0.0f };
	dest2.rotation = { 0.0f, 0.0f, 1.5f };

	settings.ms = 1500;
	settings.ease = gfx::EaseFunctions::QUADRATIC_IN;
	settings.repeats = 3;
	settings.reverseOnRepeat = true;
	queue->addComponent(std::shared_ptr<gfx::Component>(new gfx::TweenComponent(&square, dest1, dest2, settings)));

	TransformMatrix dest3 = TransformMatrix(dest2);
	dest3.scaler = { 0.05f, 0.05f, 0.0f };
	dest3.translation = { -0.5f, -0.5f, 0.0f };
	dest3.rotation = { 0.0f, 0.0f, 6.0f };

	settings.ms = 2000;
	settings.ease = gfx::EaseFunctions::LINEAR;
	settings.repeats = 1;
	settings.reverseOnRepeat = false;
	queue->addComponent(std::shared_ptr<gfx::Component>(new gfx::TweenComponent(&square, dest2, dest3, settings)));

	//have it end up at the same place
	settings.ms = 2000;
	settings.ease = gfx::EaseFunctions::ELASTIC_IN;
	settings.repeats = 3;
	settings.reverseOnRepeat = false;
	queue->addComponent(std::shared_ptr<gfx::Component>(new gfx::TweenComponent(&square, dest3, square.getTransformation(), settings)));

	square.addComponent(queue);

	win.addChild(square);

	win.getContext()->getRenderer()->setRefreshColor(Colors::DARK_BLUE);
}

int main() {
	Instance instance = Instance();
	try {
		gfx::WindowModule::LaunchConfig config = gfx::WindowModule::LaunchConfig(600, 600, "Tweening Demo");
		config.onCreate = &create;
		config.resizable = true;

		gfx::WindowModule module = gfx::WindowModule(config);
		instance.addModule(module);

		os::ErrorModule errModule = os::ErrorModule();
		instance.addModule(errModule);

		instance.start();
	} catch (const std::exception& e) {
		Error::handleError(e, instance);
		return -1;
	}
	return 0;
}
