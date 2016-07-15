#include <Catch.h>
#include <MC-System/Utility/Math.h>


namespace mc{
	namespace math
	{
		TEST_CASE("Testing isEven() and isOdd()", "[system][utility][math]") {
			SECTION("isEven()") {
				REQUIRE(isEven(4));
				REQUIRE(isEven(0));
			}

			SECTION("isOdd()") {
				REQUIRE(!isOdd(6));
				REQUIRE(isOdd(3));
				REQUIRE(!isOdd(0));
				REQUIRE(isOdd(-1));
			}
		}
		TEST_CASE("Testing roots and powers", "[system][utility][math]") {
			SECTION("Testing powers and exponenes"){
				SECTION("Testing pow()"){
					REQUIRE(pow(2, 8) == 256);
					REQUIRE(pow(5, 2) == 25);
					REQUIRE(pow(4, 3) == 64);
					REQUIRE(pow(10, 0) == 1);
					REQUIRE(pow(-6, 3) == -216);
					REQUIRE(pow(5, -2) == 0.04);
				}
				SECTION("Testing sqr()") {
					REQUIRE(sqr(2)==4);
					REQUIRE(sqr(5)==25);
					REQUIRE(sqr(-1)==1);
					REQUIRE(sqr(0)==0);
				}
				SECTION("Testing cube()") {
					REQUIRE(cube(2) == 8);
					REQUIRE(cube(5) == 125);
					REQUIRE(cube(-1) == -1);
					REQUIRE(cube(0) == 0);
				}
			}
			
			SECTION("Testing square roots") {
				REQUIRE(sqrt(25)==5);
			}
		}

		TEST_CASE("Testing ceil()", "[system][utility][math]") {
			REQUIRE(ceil(5.9) == 6);
			REQUIRE(ceil(2.1) == 3);
			REQUIRE(ceil(3) == 3);
		}

		TEST_CASE("Testing abs()", "[system][utility][math]") {
			REQUIRE(abs(5) == 5);
			REQUIRE(abs(-5) == 5);
			REQUIRE(abs(4.2) == 4.2);
			REQUIRE(abs(-4.2) == 4.2);
		}

		TEST_CASE("Testing floor()", "[system][utility][math]") {
			REQUIRE(floor(6.9) == 6);
			REQUIRE(floor(1.6) == 1);
			REQUIRE(floor(4.7) == 4);
			REQUIRE(floor(5) == 5);
			REQUIRE(floor(-5.7) == -6);
		}

		TEST_CASE("Testing isPrime()", "[system][utility][math]") {
			REQUIRE(isPrime(19));
			REQUIRE(!isPrime(360));
		}

		TEST_CASE("Testing log2()", "[system][utility][math]") {
			REQUIRE(log2(10) == 16);
			REQUIRE(log2(3) == 4);
			REQUIRE(log2(17) == 32);
		}

		TEST_CASE("Testing degrees and radians conversion", "[system][utility][math]") {
			SECTION("Testing degrees to radians") {
				REQUIRE(toRadians(118) == Approx(2.06).epsilon(0.01));
				REQUIRE(toRadians(1) == Approx(0.0174533).epsilon(0.01));
				REQUIRE(toRadians(-1) == Approx(-0.0174533).epsilon(0.01));
				REQUIRE(toRadians(360) == Approx(tau()).epsilon(0.01));
				REQUIRE(toRadians(420) == Approx(7.33038).epsilon(0.01));
			}
			SECTION("Testing radians to degrees","[system][utility][math]") {
				REQUIRE(toDegrees(tau())==Approx(360).epsilon(0.01));
				REQUIRE(toDegrees(-1)==Approx(-57.2958).epsilon(0.01));
				REQUIRE(toDegrees(3) == Approx(171.887).epsilon(0.01));
			}
		}

		TEST_CASE("Testing trigonometry functions", "[system][utility][math]") {
			/*SECTION("Testing tan()") {
				REQUIRE(tan(0)==0);
			}*/
		}
	}
}