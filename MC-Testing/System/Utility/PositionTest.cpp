#include <Catch.h>
#include <MC-System\Utility\Position.h>

namespace mc {
	TEST_CASE("Testing Coordinate", "[system][utility][position]"){
		Coordinate c = Coordinate();
		SECTION("Testing absolute") {
			REQUIRE(c.get(500)==0);
			c.setAbsolute(500);
			REQUIRE(c.get(1000)==Approx(0.0f));
			REQUIRE(c.get(500)==Approx(1.0f));
			c.setAbsolute(0);
			REQUIRE(c.get(100)==Approx(-1.0f));
			REQUIRE(c.get(1)==Approx(-1.0f));
			REQUIRE(c.get(1000)==Approx(-1.0f));
			c.setAbsolute(100);
			REQUIRE(c.get(100)==Approx(1.0f));
			REQUIRE(c.get(200) == Approx(0.0f));
			REQUIRE(c.get(300) == Approx(-0.333333f));
		}
	}
}