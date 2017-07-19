/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Catch.hpp>
#include <MACE/Graphics/Window.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/Renderer.h>
#include <MACE/Utility/MatrixTypes.h>
#include <MACE/Utility/Signal.h>

using namespace mc;


TEST_CASE("Testing creation of windows and different settings", "[graphics][window][module]") {
	Instance MACE = Instance();
	gfx::WindowModule::LaunchConfig config = gfx::WindowModule::LaunchConfig(600, 600, "Testing");
	gfx::WindowModule module = gfx::WindowModule(gfx::WindowModule::LaunchConfig(600, 600, "Testing"));

	REQUIRE_THROWS(module.setTitle("Not created yet"));
	REQUIRE(module.getName() == "MACE/Window#Testing");
	REQUIRE(module.getComponents().empty());
	REQUIRE(module.getChildren().empty());
	REQUIRE(module.getRoot() == &module);
	REQUIRE(module.getContext() == nullptr);
	REQUIRE(module.getParent() == nullptr);
	REQUIRE_FALSE(module.isDestroyed());

	REQUIRE_FALSE(MACE.moduleExists(&module));
	REQUIRE(MACE.getModule(module.getName()) == nullptr);

	MACE.addModule(module);

	REQUIRE(MACE.moduleExists(&module));
	REQUIRE(MACE.getModule(module.getName()) == &module);
}