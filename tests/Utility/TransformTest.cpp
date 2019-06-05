/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <catch2/catch.hpp>
#include <MACE/Utility/Transform.h>

namespace mc {
	namespace math {
		TEST_CASE("Testing transformation matrixes", "[utility][vector]") {
			float arr[] = { 1,2,3,1 };
			Vector<float, 4> v = arr;//this is the vector we are going to transform
			SECTION("Translating") {
				Vector<float, 4> result = translate(1, 1, 1)*v;
				REQUIRE(result[0] == 2);
				REQUIRE(result[1] == 3);
				REQUIRE(result[2] == 4);
				REQUIRE(result[3] == 1);
			}
		}

		TEST_CASE("Testing transformation class", "[utility][vector]") {
			Transformation t = Transformation();
			SECTION("Testing default values") {
				Vector<float, 4> v = { 5,-1,0,1 };
				REQUIRE(t.get()*v == v);
			}
			SECTION("Testing translation") {
				Vector<float, 4> v = { 5,-1,0,1 };
				t.translate(1, 2, 3);
				REQUIRE(t.get()*v == Vector<float, 4>({ 6,1,3,1 }));
			}
			SECTION("Testing reset") {
				t.reset();
				Vector<float, 4> v = { 5,-1,0,1 };
				REQUIRE(t.get()*v == v);
			}
		}
	}
}