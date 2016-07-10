#include <MC-Testing/Catch.h>
#include <MC-System/Utility/Vector.h>

namespace mc{
TEST_CASE("Testing matrix width and height","[system][utility][vector]") {
	Matrix<int, 5, 1> m = Matrix<int, 5, 1>();

	REQUIRE_THROWS(m.get(3,2));
	REQUIRE_THROWS(m.get(1, 4));
	REQUIRE_NOTHROW(m.get(2, 0));
	REQUIRE_THROWS(m.get(5, 0));
	REQUIRE_THROWS(m.get(4,-1));
	
	for (int x = 0; x < 5; x++) {
		REQUIRE_NOTHROW(m[x][0]);
		m[x][0] = x;
		REQUIRE(m[x][0]==x);
	}
}
TEST_CASE("Testing vector objects","[system][utility][vector]") {
	Vector5i v = Vector5i();

	for (unsigned int i = 0; i < v.size();i++) {
		v[i] = i;
		REQUIRE(v[i]==i);
		REQUIRE(v.get(i)==i);
		v.set(i,i+1);
		REQUIRE(v[i] == i + 1);
		REQUIRE(v.get(i)==i+1);
	}
}

TEST_CASE("Testing matrix objects","[system][utility][vector]") {
	Matrix5i m = Matrix5i();

	int i = 0;
	for (unsigned int x = 0; x < m.width();x++) {
		for (unsigned int y = 0; y < m.height(); y++) {
			i++;
			m[x][y] = i;
			REQUIRE(m[x][y] == i);
			REQUIRE(m.get(x,y) == i);
			m.set(x,y, i + 1);
			REQUIRE(m[x][y] == i + 1);
			REQUIRE(m.get(x,y) == i + 1);
		}
	}
}

TEST_CASE("Testing vector sizes and presets","[system][utility][vector]") {
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
	REQUIRE(m2f.size() == 4);
	Matrix3f m3f = Matrix3f();
	REQUIRE(m3f.size() == 9);
	Matrix4f m4f = Matrix4f();
	REQUIRE(m4f.size() == 16);
	Matrix5f m5f = Matrix5f();
	REQUIRE(m5f.size() == 25);

	Matrix1i m1i = Matrix1i();
	REQUIRE(m1i.size() == 1);
	Matrix2i m2i = Matrix2i();
	REQUIRE(m2i.size() == 4);
	Matrix3i m3i = Matrix3i();
	REQUIRE(m3i.size() == 9);
	Matrix4i m4i = Matrix4i();
	REQUIRE(m4i.size() == 16);
	Matrix5i m5i = Matrix5i();
	REQUIRE(m5i.size() == 25);

	Vector<int, 10> v = Vector<int, 10>();
	REQUIRE(v.size() == 10);
}
}