/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Catch.hpp>
#include <MACE/Core/System.h>
#include <MACE/Graphics/Window.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Utility/MatrixTypes.h>
#include <MACE/Utility/Signal.h>

using namespace mc;

namespace {
	bool wasCreated = false;

	void create(gfx::WindowModule& mod) {
		REQUIRE_THROWS(mod.setTitle("No window quite yet!"));
		//catch is not normally thread safe, but the render thread has a mutex lock on it by default
		REQUIRE(mod.getContext() != nullptr);
		REQUIRE_FALSE(mod.isDestroyed());
		REQUIRE(mod.isEmpty());
		REQUIRE(mod.getName() == "MACE/Window#Testing");
		REQUIRE_FALSE(mod.getProperty(gfx::Entity::INIT));
		REQUIRE_FALSE(wasCreated);
		wasCreated = true;

		SECTION("Textures") {
			gfx::Texture t = gfx::Texture();
			REQUIRE_FALSE(t.isCreated());
			REQUIRE(t.isEmpty());
			REQUIRE(t.getWidth() == 0);
			REQUIRE(t.getHeight() == 0);
			REQUIRE(t.getHue() == Colors::INVISIBLE);
			REQUIRE(t.getTransform() == Vector4f({ 0.0f, 0.0f, 1.0f, 1.0f }));

			{
				Initializer i{ t };

				REQUIRE(t.isCreated());
				REQUIRE(t.isEmpty());
				REQUIRE(t.getWidth() == 0);
				REQUIRE(t.getHeight() == 0);

				t.setData(nullptr, 1, 1);

				REQUIRE(t.isCreated());
				REQUIRE_FALSE(t.isEmpty());
				REQUIRE(t.getWidth() == 1);
				REQUIRE(t.getHeight() == 1);

				MACE_CONSTEXPR const int data[3][3] =
				{
					{ 1, 2, 3 },
					{ 1, 2, 3 },
					{ 1, 2, 3 }
				};

				t.setData(data, gfx::Enums::Format::RGB, gfx::Enums::InternalFormat::RGB);

				REQUIRE(t.isCreated());
				REQUIRE_FALSE(t.isEmpty());
				REQUIRE(t.getWidth() == 3);
				REQUIRE(t.getHeight() == 3);
				REQUIRE(t.getType() == gfx::Enums::Type::INT);
				REQUIRE(t.getFormat() == gfx::Enums::Format::RGB);
				REQUIRE(t.getInternalFormat() == gfx::Enums::InternalFormat::RGB);
				/*
				int readData[9] = {};

				t.readPixels(readData);

				for (unsigned int x = 0; x < os::getArraySize(data); ++x) {
				for (unsigned int y = 0; y < os::getArraySize(data[x]); ++y) {
				REQUIRE(readData[y + (x * 3)] == data[x][y]);
				}
				}*/
			}

			REQUIRE_FALSE(t.isCreated());
			REQUIRE(t.isEmpty());
			REQUIRE(t.getWidth() == 0);
			REQUIRE(t.getHeight() == 0);
		}
	}
}


TEST_CASE("Testing creation of windows and different settings", "[graphics][window][module]") {
	Instance MACE = Instance();
	gfx::WindowModule::LaunchConfig config = gfx::WindowModule::LaunchConfig(600, 600, "Testing");
	config.onCreate = &create;
	config.fps = 100;
	config.resizable = true;
	config.decorated = false;
	gfx::WindowModule module = gfx::WindowModule(config);

	REQUIRE_THROWS(module.setTitle("Not created yet"));
	REQUIRE(module.getName() == "MACE/Window#Testing");
	REQUIRE(module.getComponents().empty());
	REQUIRE(module.getChildren().empty());
	REQUIRE(module.getRoot() == &module);
	REQUIRE(module.getContext() == nullptr);
	REQUIRE(module.getParent() == nullptr);
	REQUIRE_FALSE(module.isDestroyed());

	REQUIRE(module.getInstance() == nullptr);
	REQUIRE_FALSE(MACE.moduleExists(&module));
	REQUIRE(MACE.getModule(module.getName()) == nullptr);

	MACE.addModule(module);

	os::ErrorModule errorModule = os::ErrorModule();
	MACE.addModule(errorModule);

	REQUIRE(*module.getInstance() == MACE);
	REQUIRE(MACE.moduleExists(&module));
	REQUIRE(MACE.getModule(module.getName()) == &module);

	REQUIRE_FALSE(wasCreated);

	MACE.init();

	REQUIRE(MACE.moduleExists(&module));
	REQUIRE(MACE.getModule(module.getName()) == &module);

	for (Index i = 0; i < 10; ++i) {
		MACE.update();
		REQUIRE(MACE.isRunning());
	}

	REQUIRE(MACE.isRunning());

	MACE.requestStop();

	REQUIRE_FALSE(MACE.isRunning());

	MACE.destroy();

	REQUIRE(wasCreated);
}