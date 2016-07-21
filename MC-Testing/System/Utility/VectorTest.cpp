#include <Catch.h>
#include <MC-System/Utility/Vector.h>

namespace mc{
TEST_CASE("Testing matrix width and height","[system][utility][vector]") {
	Matrix<int, 5, 1> m = Matrix<int, 5, 1>();

	SECTION("Testing if it throws IndexOutOfBounds"){
		REQUIRE_THROWS(m.get(3,2));
		REQUIRE_THROWS(m.get(1, 4));
		REQUIRE_NOTHROW(m.get(2, 0));
		REQUIRE_THROWS(m.get(5, 0));
		REQUIRE_THROWS(m.get(4,-1));
	}
	
	SECTION("Testing if the [] operator works"){
		for (int x = 0; x < 5; x++) {
			REQUIRE_NOTHROW(m[x][0]);
			m[x][0] = x;
			REQUIRE(m[x][0]==x);
		}
	}
}

TEST_CASE("Testing vector and matrix initialiation", "[system][utility][vector]") {
	SECTION("Vector intialization") {
		Vector3f v;
		SECTION("Testing assigning vector to an array"){
			float arr[] = { 1.0f,42.0f,100.0f };
			Vector3f v = arr;
			REQUIRE(v[0]==1.0f);
			REQUIRE(v[1]==42.0f);
			REQUIRE(v[2]==100.0f);
		}
		SECTION("Testing default constructor"){
			v = Vector3f();
			REQUIRE(v[0]==0);
			REQUIRE(v[1]==0);
			REQUIRE(v[2]==0);
		}
		SECTION("Testing contructor with array"){
			float arr[] = {45.512f,6.9f,4.20f};
			v = Vector3f(arr);
			REQUIRE(v.get(0)==45.512f);
			REQUIRE(v.get(1)==6.9f);
			REQUIRE(v.get(2)==4.20f);
		}
		SECTION("Testing contructor with std::array") {
			std::array<float,3> arr = {-41.7f,100.9f,1.1f};
			v = Vector3f(arr);
			REQUIRE(v.get(0)==(-41.7f));
			REQUIRE(v.get(1)==100.9f);
			REQUIRE(v.get(2)==1.1f);
		}
		SECTION("Testing copy constructors") {
			v = Vector3f();
			v.set(0, 67.9f);
			v.set(1, 56.0f);
			v.set(2, 42.4f);
			SECTION("Testing Vector(Vector&)"){
				Vector3f v2 = Vector3f(v);
				REQUIRE(v.get(0) == 67.9f);
				REQUIRE(v.get(1) == 56.0f);
				REQUIRE(v.get(2) == 42.4f);
			}
			SECTION("Testing copy assignment"){
				Vector3f v3 = v;//no constructor, just assignment. compiler should use the copy contructor
				REQUIRE(v[0]==67.9f);
				REQUIRE(v[1]==56.0f);
				REQUIRE(v[2]==42.4f);
			}
		}
	}
}

TEST_CASE("Testing getting and setting","[system][utility][vector]") {
	SECTION("Testing getting and setting of a vector") {
		Vector5i v = Vector5i();

		for (Index i = 0; i < v.size(); i++) {
			v[i] = i;
			REQUIRE(v[i] == i);
			REQUIRE(v.get(i) == i);
			v.set(i, i + 1);
			REQUIRE(v[i] == i + 1);
			REQUIRE(v.get(i) == i + 1);
		}
	}

	SECTION("Testing getting and setting of matrices") {
		Matrix5i m = Matrix5i();

		int i = 0;
		for (Index x = 0; x < m.width(); x++) {
			for (Index y = 0; y < m.height(); y++) {
				i++;
				m[x][y] = i;
				REQUIRE(m[x][y] == i);
				REQUIRE(m.get(x, y) == i);
				m.set(x, y, i + 1);
				REQUIRE(m[x][y] == i + 1);
				REQUIRE(m.get(x, y) == i + 1);
			}
		}
	}
}

TEST_CASE("Testing vector sizes and presets","[system][utility][vector]") {
	SECTION("Testing Vector size()"){
		Vector<int, 10> v = Vector<int, 10>();
		REQUIRE(v.size() == 10);

		SECTION("Testing vector float presets"){
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
		}
		SECTION("Testing vector int presets"){
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
		}
	}
	SECTION("Testing Matrix size(), width(), and height()"){
		Matrix<int, 3, 7> m = Matrix<int,3,7>();
		REQUIRE(m.size()==21);
		REQUIRE(m.width()==3);
		REQUIRE(m.height()==7);

		SECTION("Testing Matrix float presets"){
			Matrix1f m1f = Matrix1f();
			REQUIRE(m1f.size() == 1);
			REQUIRE(m1f.width()==1);
			REQUIRE(m1f.height()==1);
			Matrix2f m2f = Matrix2f();
			REQUIRE(m2f.size() == 4);
			REQUIRE(m2f.width() == 2);
			REQUIRE(m2f.height() == 2);
			Matrix3f m3f = Matrix3f();
			REQUIRE(m3f.size() == 9);
			REQUIRE(m3f.width() == 3);
			REQUIRE(m3f.height() == 3);
			Matrix4f m4f = Matrix4f();
			REQUIRE(m4f.size() == 16);
			REQUIRE(m4f.width() == 4);
			REQUIRE(m4f.height() == 4);
			Matrix5f m5f = Matrix5f();
			REQUIRE(m5f.size() == 25);
			REQUIRE(m5f.width() == 5);
			REQUIRE(m5f.height() == 5);
		}
		SECTION("Testing Matrix int presets"){
			Matrix1i m1i = Matrix1i();
			REQUIRE(m1i.size() == 1);
			REQUIRE(m1i.width() == 1);
			REQUIRE(m1i.height() == 1);
			Matrix2i m2i = Matrix2i();
			REQUIRE(m2i.size() == 4);
			REQUIRE(m2i.width() == 2);
			REQUIRE(m2i.height() == 2);
			Matrix3i m3i = Matrix3i();
			REQUIRE(m3i.size() == 9);
			REQUIRE(m3i.width() == 3);
			REQUIRE(m3i.height() == 3);
			Matrix4i m4i = Matrix4i();
			REQUIRE(m4i.size() == 16);
			REQUIRE(m4i.width() == 4);
			REQUIRE(m4i.height() == 4);
			Matrix5i m5i = Matrix5i();
			REQUIRE(m5i.size() == 25);
			REQUIRE(m5i.width() == 5);
			REQUIRE(m5i.height() == 5);
		}
	}
}
}