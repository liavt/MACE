#include <Catch.h>
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

TEST_CASE("Testing nextDigitOf2()","[system][utility][math]") {
	REQUIRE(Math::nextDigitOf2(10)==16);
	REQUIRE(Math::nextDigitOf2(3) == 4);
	REQUIRE(Math::nextDigitOf2(17)==32);
}
}