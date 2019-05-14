/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <catch2/catch.hpp>
#include <MACE/Utility/Color.h>

namespace mc {
	TEST_CASE("Testing color initialization", "[color][utility]") {
		Color c = { 0.0f,0.64f,0.12f,1.0f };
		REQUIRE(c.r == 0.0f);
		REQUIRE(c.g == 0.64f);
		REQUIRE(c.b == 0.12f);
		REQUIRE(c.a == 1.0f);
	}

	TEST_CASE("Testing color equality and copy constructors", "[utility][color]") {
		Color c1 = { 0.0f,0.5f,0.3f,0.4f };
		Color c2 = { 1.0f,0.5f,0.7f,1.0f };
		REQUIRE_FALSE(c1 == c2);
		REQUIRE(c1 != c2);
		c1 = c2;
		REQUIRE(c1 == c2);
		REQUIRE_FALSE(c1 != c2);
	}

	TEST_CASE("Testing color unsigned int converstion", "[utility][color]") {
		REQUIRE(Color(Colors::RED.toUnsignedInt()) == Colors::RED);
		REQUIRE(Color(Colors::WHITE.toUnsignedInt()) == Colors::WHITE);
		REQUIRE(Color(Colors::CYAN.toUnsignedInt()) == Colors::CYAN);
		REQUIRE(Color(Colors::BLACK.toUnsignedInt()) == Colors::BLACK);

		REQUIRE(Color(255, 122, 15, 255).toUnsignedInt() == 4286189567/*0xFF7A0FFF*/);
		REQUIRE(Color(4286189567) == Color(255, 122, 15, 255));
	}

	TEST_CASE("Testing color begin() and end() and iterations", "[utility][color]") {
		const float values[4] = { 0.1f, 0.534f, 0.98f, 1.0f };

		const Color col = values;
		const Vector<float, 4> vec = col.toVector();

		Index iterator = 0;
		for (const float f : col) {
			const float expected = values[iterator];
			REQUIRE(f == expected);
			REQUIRE(vec[iterator] == expected);
			REQUIRE(col.getComponent(iterator) == expected);
			REQUIRE(col[iterator] == expected);
			++iterator;
		}

		REQUIRE_THROWS(col.getComponent(5));
		REQUIRE(iterator == col.size());
	}

	TEST_CASE("Testing hexcodes", "[utility][color]") {
		REQUIRE(fromHex(Colors::MAGENTA.toHex()) == Colors::MAGENTA);
		REQUIRE(fromHex(Colors::CYAN.toHex()) == Colors::CYAN);
		REQUIRE(fromHex(Colors::WHITE.toHex()) == Colors::WHITE);

		REQUIRE(Color(255, 122, 15, 255).toHex() == "ff7a0fff");
		REQUIRE(fromHex("FF7A0FFF") == Color(255, 122, 15, 255));
	}

	TEST_CASE("Testing color storage", "[color][utility][slow]") {
		Color c = Color(0.0f, 0.0f, 0.0f, 1.0f);
		REQUIRE(c.getRed() == 0);
		REQUIRE(c.getGreen() == 0);
		REQUIRE(c.getBlue() == 0);
		REQUIRE(c.getAlpha() == 255);
		REQUIRE(c.r == 0);
		REQUIRE(c.g == 0);
		REQUIRE(c.b == 0);
		REQUIRE(c.a == 1.0f);

		for (unsigned int i = 0; i < 250; i += 10) {
			c.setRed(i);
			REQUIRE(c.getRed() == i);
			REQUIRE(c.r == Approx((float)i / 255.0f));

			c.setGreen(i);
			REQUIRE(c.getGreen() == i);
			REQUIRE(c.g == Approx((float)i / 255.0f));

			c.setBlue(i);
			REQUIRE(c.getBlue() == i);
			REQUIRE(c.b == Approx((float)i / 255.0f));

			c.setAlpha(i);
			REQUIRE(c.getAlpha() == i);
			REQUIRE(c.a == Approx((float)i / 255.0f));
		}
	}
}