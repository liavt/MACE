#include <Catch.h>
#include <MC-System/Utility/Math.h>


namespace mc{
	TEST_CASE("Testing isEven() and isOdd()", "[system][utility][math]") {
		SECTION("isEven()") {
			REQUIRE(math::isEven(4));
			REQUIRE(math::isEven(0));
		}

		SECTION("isOdd()") {
			REQUIRE(!math::isOdd(6));
			REQUIRE(math::isOdd(3));
			REQUIRE(!math::isOdd(0));
			REQUIRE(math::isOdd(-1));
		}
	}
	TEST_CASE("Testing pow()","[system][utility][math]") {
		REQUIRE(math::pow(2,8)==256);
		REQUIRE(math::pow(5,2)==25);
		REQUIRE(math::pow(4,3)==64);
		REQUIRE(math::pow(10,0)==1);
		REQUIRE(math::pow(-6,3)==-216);
		REQUIRE(math::pow(5,-2)==0.04);
	}

	TEST_CASE("Testing ceil()","[system][utility][math]") {
		REQUIRE(math::ceil(5.9)==6);
		REQUIRE(math::ceil(2.1)==3);
		REQUIRE(math::ceil(3)==3);
	}

	TEST_CASE("Testing abs()","[system][utility][math]") {
		REQUIRE(math::abs(5)==5);
		REQUIRE(math::abs(-5)==5);
		REQUIRE(math::abs(4.2)==4.2);
		REQUIRE(math::abs(-4.2)==4.2);
	}

	TEST_CASE("Testing floor()","[system][utility][math]") {
		REQUIRE(math::floor(6.9) == 6);
		REQUIRE(math::floor(1.6) == 1);
		REQUIRE(math::floor(4.7)==4);
		REQUIRE(math::floor(5)==5);
		REQUIRE(math::floor(-5.7)==-6);
	}

	TEST_CASE("Testing isPrime()","[system][utility][math]") {
		REQUIRE(math::isPrime(19));
		REQUIRE(!math::isPrime(360));
	}

	TEST_CASE("Testing log2()","[system][utility][math]") {
		REQUIRE(math::log2(10)==16);
		REQUIRE(math::log2(3) == 4);
		REQUIRE(math::log2(17)==32);
	}

	TEST_CASE("Testing trigonometry functions","[system][utility][math]") {
		/*SECTION("Testing tan()") {
			REQUIRE(math::tan(0)==0);
		}*/
	}
}