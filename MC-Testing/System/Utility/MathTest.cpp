#include <MC-Testing/Catch.h>
#include <MC-System/Utility/Math.h>


namespace mc{
TEST_CASE("Testing floor()","[system][utility][math]") {
	REQUIRE(Math::floor(6.9) == 6);
	REQUIRE(Math::floor(1.6) == 1);
	REQUIRE(Math::floor(4.7)==4);
	REQUIRE(Math::floor(-5.7)==-5);
}

TEST_CASE("Testing isPrime()","[system][utility][math]") {
	REQUIRE(Math::isPrime(19));
	REQUIRE(!Math::isPrime(360));
}

TEST_CASE("Testing nextDoubleOf2()","[system][utility][math]") {
	REQUIRE(Math::nextDoubleOf2(10)==16);
	REQUIRE(Math::nextDoubleOf2(3) == 4);
	REQUIRE(Math::nextDoubleOf2(17)==32);
}
}