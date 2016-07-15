#include <Catch.h>
#include <MC-System/Utility/Color.h>

namespace mc{
TEST_CASE("Testing color conversion methods","[system][color][utility]") {
	REQUIRE(mc::Color::trimFloat(1.1f)==Approx(1.0f));
	REQUIRE(mc::Color::trimFloat(-0.1f) == Approx(0.0f));
	REQUIRE(mc::Color::trimFloat(0.5f) == Approx(0.5f));

	REQUIRE(mc::Color::convertFloatToRGBA(1.0f)==254);
	REQUIRE(mc::Color::convertRGBAToFloat(254) == Approx(1.0f).epsilon(0.01));

	REQUIRE(mc::Color::convertFloatToRGBA(0.0f) == 0);
	REQUIRE(mc::Color::convertRGBAToFloat(0) == Approx(0.0f).epsilon(0.01));

	REQUIRE(mc::Color::convertFloatToRGBA(0.5f) == 127);
	REQUIRE(mc::Color::convertRGBAToFloat(127) == Approx(0.5f).epsilon(0.01));
}

TEST_CASE("Testing color storage","[system][color][utility][slow]") {
	Color c = Color(0.0f,0.0f,0.0f,1.0f);

	REQUIRE(c.getRed()==0);
	REQUIRE(c.getGreen() == 0);
	REQUIRE(c.getBlue() == 0);
	REQUIRE(c.getAlpha() == 254);
	REQUIRE(c.r== 0);
	REQUIRE(c.g == 0);
	REQUIRE(c.b== 0);
	REQUIRE(c.a == 1.0f);

	for (unsigned int i = 0; i < 254; i++) {
		c.setRed(i);
		REQUIRE(c.getRed()==i);
		REQUIRE(c.r==Approx((float)i/254.0f));

		c.setGreen(i);
		REQUIRE(c.getGreen() == i);
		REQUIRE(c.g== Approx((float)i / 254.0f));

		c.setBlue(i);
		REQUIRE(c.getBlue() == i);
		REQUIRE(c.b	== Approx((float)i / 254.0f));

		c.setAlpha(i);
		REQUIRE(c.getAlpha() == i);
		REQUIRE(c.a == Approx((float)i / 254.0f));
	}
}
}