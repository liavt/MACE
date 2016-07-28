#include <Catch.h>
#include <MC-System/Utility/Transform.h>

namespace mc{
namespace math{
TEST_CASE("Testing transformation matrixes") {
	float arr[] = {1,2,3,1};
	Vector4f v = arr;//this is the vector we are going to transform
	SECTION("Translating") {
		Vector4f result = translate(1, 1, 1)*v;
		REQUIRE(result[0]==2);
		REQUIRE(result[1] == 3);
		REQUIRE(result[2] == 4);
		REQUIRE(result[3]==1);
	}
}
}
}