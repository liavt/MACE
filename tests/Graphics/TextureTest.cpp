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
#include <MACE/Graphics/Renderer.h>
#include <MACE/Utility/MatrixTypes.h>
#include <MACE/Utility/Signal.h>

using namespace mc;

namespace {
	void create(gfx::WindowModule& mod) {
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
				{1, 2, 3},
				{1, 2, 3},
				{1, 2, 3}
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


TEST_CASE("Testing textures", "[graphics][window][texture]") {
	Instance MACE = Instance();
	gfx::WindowModule::LaunchConfig config = gfx::WindowModule::LaunchConfig(40, 1, "Testing");
	config.onCreate = &create;
	gfx::WindowModule module = gfx::WindowModule(config);

	MACE.addModule(module);

	Initializer i{ MACE };
}