#include <MC-Testing/Catch.h>
#include <MC-System/Utility/Vector.h>

namespace mc{


TEST_CASE("Testing vector size","[system][utility][vector]") {
	Vector1f v1f = Vector1f();
	REQUIRE(v1f.size()==1);
	Vector2f v2f = Vector2f();
	REQUIRE(v2f.size() == 2);
	Vector3f v3f = Vector3f();
	REQUIRE(v3f.size() == 3);
	Vector4f v4f = Vector4f();
	REQUIRE(v4f.size() == 4);
	Vector5f v5f = Vector5f();
	REQUIRE(v5f.size() == 5);

	Vector1i v1i = Vector1i();
	REQUIRE(v1i.size() == 1);
	Vector2i v2i = Vector2i();
	REQUIRE(v2i.size() == 2);
	Vector3i v3i = Vector3i();
	REQUIRE(v3i.size() == 3);
	Vector4i v4i = Vector4i();
	REQUIRE(v4i.size() == 4);
	Vector5i v5i = Vector5i();
	REQUIRE(v5i.size() == 5);

	Matrix1f m1f = Matrix1f();
	REQUIRE(m1f.size() == 1);
	Matrix2f m2f = Matrix2f();
	REQUIRE(m2f.size() == 2);
	Matrix3f m3f = Matrix3f();
	REQUIRE(m3f.size() == 3);
	Matrix4f m4f = Matrix4f();
	REQUIRE(m4f.size() == 4);
	Matrix5f m5f = Matrix5f();
	REQUIRE(m5f.size() == 5);

	Matrix1i m1i = Matrix1i();
	REQUIRE(m1i.size() == 1);
	Matrix2i m2i = Matrix2i();
	REQUIRE(m2i.size() == 2);
	Matrix3i m3i = Matrix3i();
	REQUIRE(m3i.size() == 3);
	Matrix4i m4i = Matrix4i();
	REQUIRE(m4i.size() == 4);
	Matrix5i m5i = Matrix5i();
	REQUIRE(m5i.size() == 5);

	Vector<int, 10> v = Vector<int, 10>();
	REQUIRE(v.size() == 10);
}
}