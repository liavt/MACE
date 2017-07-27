/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Catch.hpp>
#include <MACE/Utility/Matrix.h>
#include <MACE/Utility/MatrixTypes.h>

namespace mc {
	TEST_CASE("Testing matrix width and height", "[utility][vector]") {
		Matrix<int, 5, 1> m = Matrix<int, 5, 1>();

		SECTION("Testing if it throws IndexOutOfBounds") {
			REQUIRE_THROWS(m.get(3, 2));
			REQUIRE_THROWS(m.get(1, 4));
			REQUIRE_NOTHROW(m.get(2, 0));
			REQUIRE_THROWS(m.get(5, 0));
			REQUIRE_THROWS(m.get(4, -1));
		}

		SECTION("Testing () operator") {
			for (Index x = 1; x <= 5; x++) {
				REQUIRE_NOTHROW(m(x, 1));
				m(x, 1) = x;
				REQUIRE(m(x, 1) == x);
			}
		}

		SECTION("Testing if the [] operator works") {
			for (Index x = 0; x < 5; x++) {
				REQUIRE_NOTHROW(m[x][0]);
				m[x][0] = x;
				REQUIRE(m[x][0] == x);
			}
		}
	}

	TEST_CASE("Testing operators", "[utility][vector]") {
		SECTION("Vector operators") {
			SECTION("Equality and inequality") {
				Vector4i v1 = { 1,4,3,5 };
				Vector4i v2 = { 3,4,6,3 };
				REQUIRE_FALSE(v1 == v2);
				REQUIRE(v1 != v2);
				v1 = v2;
				REQUIRE(v1 == v2);
				REQUIRE_FALSE(v1 != v2);
			}
			SECTION("Greater than and less than") {
				Vector4i v1 = { 1,4,3,5 };
				Vector4i v2 = { 3,5,6,6 };
				REQUIRE_FALSE(v1 > v2);
				REQUIRE_FALSE(v1 >= v2);
				REQUIRE(v1 < v2);
				REQUIRE(v1 <= v2);
				v2 = v1;
				REQUIRE_FALSE(v1 > v2);
				REQUIRE(v1 >= v2);
				REQUIRE_FALSE(v1 < v2);
				REQUIRE(v1 <= v2);
				v2 = { 0,3,2,4 };
				REQUIRE(v1 > v2);
				REQUIRE(v1 >= v2);
				REQUIRE_FALSE(v1 < v2);
				REQUIRE_FALSE(v1 <= v2);
			}
		}
		SECTION("Matrix operator") {
			SECTION("Equality and inequality") {
				Matrix3i m1 = { {1,3,4},{5,4,6},{4,3,4} };
				Matrix3i m2 = { { 4,5,3},{ 2,1,6 },{ 3,1,-1 } };
				REQUIRE_FALSE(m1 == m2);
				REQUIRE(m1 != m2);
				m1 = m2;
				REQUIRE(m1 == m2);
				REQUIRE_FALSE(m1 != m2);
			}
			SECTION("Greater than and less than") {
				Matrix3i m1 = { { 1,3,4 },{ 5,4,6 },{ 4,3,4 } };
				Matrix3i m2 = { { 4,5,6 },{ 6,5,8 },{ 7,5,6 } };
				REQUIRE_FALSE(m1 > m2);
				REQUIRE_FALSE(m1 >= m2);
				REQUIRE(m1 < m2);
				REQUIRE(m1 <= m2);
				REQUIRE(m1 != m2);
				REQUIRE_FALSE(m1 == m2);
				m2 = m1;
				REQUIRE_FALSE(m1 > m2);
				REQUIRE(m1 >= m2);
				REQUIRE_FALSE(m1 < m2);
				REQUIRE(m1 <= m2);
				REQUIRE(m1 == m2);
				m2 = { {-1,0,2},{3,1,4},{2,1,2} };
				REQUIRE(m1 > m2);
				REQUIRE(m1 >= m2);
				REQUIRE_FALSE(m1 < m2);
				REQUIRE_FALSE(m1 <= m2);
				REQUIRE(m1 != m2);
			}
		}
	}

	TEST_CASE("Testing vector and matrix initialiation", "[utility][vector]") {
		SECTION("Vector intialization") {
			SECTION("Testing assigning vector to an array") {
				float arr[] = { 1.0f,42.0f,100.0f };
				Vector3f v = arr;
				REQUIRE(v[0] == 1.0f);
				REQUIRE(v[1] == 42.0f);
				REQUIRE(v[2] == 100.0f);
			}
			SECTION("Testing default constructor") {
				Vector3f v = Vector3f();
				REQUIRE(v[0] == 0);
				REQUIRE(v[1] == 0);
				REQUIRE(v[2] == 0);
			}
			SECTION("Testing contructor with array") {
				float arr[] = { 45.512f,6.9f,4.20f };
				Vector3f v = Vector3f(arr);
				REQUIRE(v.get(0) == 45.512f);
				REQUIRE(v.get(1) == 6.9f);
				REQUIRE(v.get(2) == 4.20f);
			}
			SECTION("Testing contructor with std::array") {
				std::array<float, 3> arr = { -41.7f,100.9f,1.1f };
				Vector3f v = Vector3f(arr);
				REQUIRE(v.get(0) == (-41.7f));
				REQUIRE(v.get(1) == 100.9f);
				REQUIRE(v.get(2) == 1.1f);
			}
			SECTION("Testing aggregate constructor") {
				SECTION("Constructor-stlye") {
					Vector4i v = Vector4i({ 4,1,32,5 });
					REQUIRE(v[0] == 4);
					REQUIRE(v[1] == 1);
					REQUIRE(v[2] == 32);
					REQUIRE(v[3] == 5);
				}
				SECTION("Aggregate-style") {
					Vector4i v = { 4,1,32,5 };
					REQUIRE(v[0] == 4);
					REQUIRE(v[1] == 1);
					REQUIRE(v[2] == 32);
					REQUIRE(v[3] == 5);
				}
			}
			SECTION("Testing copy constructors") {
				Vector3f v = Vector3f();
				v.set(0, 67.9f);
				v.set(1, 56.0f);
				v.set(2, 42.4f);
				SECTION("Testing Vector(Vector&)") {
					Vector3f v2 = Vector3f(v);
					REQUIRE(v.get(0) == 67.9f);
					REQUIRE(v.get(1) == 56.0f);
					REQUIRE(v.get(2) == 42.4f);
				}
				SECTION("Testing copy assignment") {
					Vector3f v3 = v;//no constructor, just assignment. compiler should use the copy contructor
					REQUIRE(v[0] == 67.9f);
					REQUIRE(v[1] == 56.0f);
					REQUIRE(v[2] == 42.4f);
				}
			}

			SECTION("Type conversion") {
				Vector1i v1 = { 2 };
				REQUIRE(v1.size() == 1);
				int content = static_cast<int>(v1);
				REQUIRE(content == 2);

				v1 = 3;
				content = v1;
				REQUIRE(content == 3);
			}
		}

		SECTION("Matrix initialization") {
			SECTION("Testing creating Matrix from array") {
				int arr[3][3] = { {1,2,3},{4,5,6},{7,8,9} };
				Matrix3i m = arr;
				Index counter = 0;
				for (Index x = 0; x < m.width(); x++) {
					for (Index y = 0; y < m.height(); y++) {
						counter++;
						REQUIRE(m[x][y] == counter);
					}
				}
			}
			SECTION("Testing default constructor") {
				Matrix2f m = Matrix2f();
				REQUIRE(m[0][0] == 0);
				REQUIRE(m[0][1] == 0);
				REQUIRE(m[1][0] == 0);
				REQUIRE(m[1][1] == 0);
			}
			SECTION("Testing constructor with array") {
				int arr[3][3] = { { 1,2,3 },{ 4,5,6 },{ 7,8,9 } };
				Matrix3i m = Matrix3i(arr);
				Index counter = 0;
				for (Index x = 0; x < m.width(); x++) {
					for (Index y = 0; y < m.height(); y++) {
						counter++;
						REQUIRE(m[x][y] == counter);
					}
				}
			}
			SECTION("Testing aggregate style constructor") {
				SECTION("Constructor style") {
					const Matrix3i m = Matrix3i({ { 1,2,3 },{ 1,2,3 },{ 1,2,3 } });
					REQUIRE(m[0][0] == 1);
					REQUIRE(m[0][1] == 2);
					REQUIRE(m[0][2] == 3);
					REQUIRE(m[1][0] == 1);
					REQUIRE(m[1][1] == 2);
					REQUIRE(m[1][2] == 3);
					REQUIRE(m[2][0] == 1);
					REQUIRE(m[2][1] == 2);
					REQUIRE(m[2][2] == 3);
				}
				SECTION("Assignment style") {
					const Matrix3i m = { {1,2,3}, {1,2,3}, {1,2,3} };
					REQUIRE(m[0][0] == 1);
					REQUIRE(m[0][1] == 2);
					REQUIRE(m[0][2] == 3);
					REQUIRE(m[1][0] == 1);
					REQUIRE(m[1][1] == 2);
					REQUIRE(m[1][2] == 3);
					REQUIRE(m[2][0] == 1);
					REQUIRE(m[2][1] == 2);
					REQUIRE(m[2][2] == 3);
				}
			}
			SECTION("Testing copy constructors") {
				Matrix4i m = Matrix4i();
				for (Index x = 0; x < m.width(); x++) {
					for (Index y = 0; y < m.height(); y++) {
						m[x][y] = x*y;
					}
				}
				SECTION("Testing via explicit constructor") {
					Matrix4i other = Matrix4i(m);
					for (Index x = 0; x < other.width(); x++) {
						for (Index y = 0; y < other.height(); y++) {
							REQUIRE(other[x][y] == x*y);
						}
					}
				}
				SECTION("Testing via assignment") {
					Matrix4i other = m;
					for (Index x = 0; x < other.width(); x++) {
						for (Index y = 0; y < other.height(); y++) {
							REQUIRE(other[x][y] == x*y);
						}
					}
				}
			}
		}
	}

	TEST_CASE("Testing getting and setting", "[utility][vector]") {
		SECTION("Testing getting and setting of a vector") {
			Vector5i v = Vector5i();

			for (Index i = 0; i < v.size(); i++) {
				v[i] = i;
				REQUIRE(v[i] == i);
				REQUIRE(v.get(i) == i);
				REQUIRE(v(i + 1) == i);
				v.set(i, i + 1);
				REQUIRE(v[i] == i + 1);
				REQUIRE(v.get(i) == i + 1);
				REQUIRE(v(i + 1) == i + 1);
				v(i + 1) = i + 2;
				REQUIRE(v[i] == i + 2);
				REQUIRE(v.get(i) == i + 2);
				REQUIRE(v(i + 1) == i + 2);
			}
		}

		SECTION("Testing GLSL-style getting") {
			Vector4i v = { 1,2,3,4 };
			REQUIRE(v.x() == Vector1i({ 1 }));
			REQUIRE(v.y() == Vector1i({ 2 }));
			REQUIRE(v.z() == Vector1i({ 3 }));
			REQUIRE(v.w() == Vector1i({ 4 }));
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
					REQUIRE(m(x + 1, y + 1) == i);
					m.set(x, y, i + 1);
					REQUIRE(m[x][y] == i + 1);
					REQUIRE(m.get(x, y) == i + 1);
					REQUIRE(m(x + 1, y + 1) == i + 1);
					m(x + 1, y + 1) = i + 2;
					REQUIRE(m[x][y] == i + 2);
					REQUIRE(m.get(x, y) == i + 2);
					REQUIRE(m(x + 1, y + 1) == i + 2);
				}
			}
		}
	}

	TEST_CASE("Testing math", "[utility][vector]") {//eww math
		SECTION("Vector math") {
			SECTION("Adding") {
				Vector3i v1 = { 1,0,3 };
				Vector3i v2 = { -1,4,2 };
				Vector3i result = { 0,4,5 };
				REQUIRE(v1 + v2 == result);
			}
			SECTION("Subtracting") {
				Vector3i v1 = { 1,0,3 };
				Vector3i v2 = { -1,4,2 };
				Vector3i result = { 2,-4,1 };
				REQUIRE(v1 - v2 == result);
			}
			SECTION("Multiplying") {
				Vector3i v1 = { 1,0,3 };
				Vector3i v2 = { -1,4,2 };
				Vector3i result = { -1,0,6 };
				REQUIRE(v1*v2 == result);
			}
			SECTION("Dot product") {
				Vector3i v1 = { 1,0,3 };
				Vector3i v2 = { -1,4,2 };
				REQUIRE(math::dot(v1, v2) == 5);
			}
			SECTION("Cross product") {
				Vector3i v1 = { 1,0,3 };
				Vector3i v2 = { -1,4,2 };
				REQUIRE(math::cross(v1, v2) == Vector3i({ -12,-5,4 }));
			}
			SECTION("Magnitudes") {
				Vector3f v1 = { 1,0,3 };
				Vector3f v2 = { -1,4,2 };
				REQUIRE(math::magnitude(v1) == Approx(3.162277));
				REQUIRE(math::magnitude(v2) == Approx(4.582575));
			}
			SECTION("Normalizing") {
				Vector3f input = { 3,-4,0 };
				Vector3f output = { (3.0f / 5.0f),-(4.0f / 5.0f),0 };
				REQUIRE(math::normalize(input) == output);
			}
			SECTION("Scalar multiplication") {
				REQUIRE(Vector3f({ 5,-9,11 }) * 1 == Vector3f({ 5,-9,11 }));
				REQUIRE(Vector3f({ 5,-9,11 }) * 2 == Vector3f({ 10,-18,22 }));
				REQUIRE(Vector3f({ 5,-9,11 }) * -2 == Vector3f({ -10,18,-22 }));
			}
		}

		SECTION("Matrix math") {
			SECTION("Matrix by Matrix") {
				SECTION("Adding") {
					REQUIRE(Matrix3i({ {1,2,3},{4,5,6},{7,8,9} }) + Matrix3i({ {3,3,12},{4,5,6},{3,7,8} }) == Matrix3i({ {4,5,15},{8,10,12},{10,15,17} }));
				}
				SECTION("Subtracting") {
					REQUIRE(Matrix3i({ { 1,2,3 },{ 4,5,6 },{ 7,8,9 } }) - Matrix3i({ { 3,3,12 },{ 4,5,6 },{ 3,7,8 } }) == Matrix3i({ { -2,-1,-9 },{ 0,0,0 },{ 4,1,1 } }));

				}
				SECTION("Multiplying") {
					REQUIRE(Matrix3i({ { 1,2,3 },{ 4,5,6 },{ 7,8,9 } }) * Matrix3i({ { 3,3,12 },{ 4,5,6 },{ 3,7,8 } }) == Matrix3i({ { 20,34,48 },{ 50,79,126 },{ 80,124,204 } }));
				}
			}
			SECTION("Matrix by Vector") {
				SECTION("Multiplying") {
					REQUIRE(Matrix3i({ { 1,3,2 },{ 4,-1,5 },{ 0,6,7 } }) * Vector3i({ 3,-2,0 }) == Vector3i({ -5,2,0 }));
				}
			}
			SECTION("Additional Matrix math") {
				SECTION("Transpose") {
					Matrix<float, 3, 2> result = math::transpose(Matrix<float, 2, 3>({ { 3,4,0 },{ -1,3,2 } }));
					Matrix<float, 3, 2> actual = Matrix<float, 3, 2>({ { 3,-1 },{ 4,3 },{ 0,2 } });
					REQUIRE(result == actual);
				}
				SECTION("Determinate") {
					//how determinated am i to determine the determinate of a matrix?
					REQUIRE(math::det(Matrix2f({ {1,3},{5,4} })) == -11);
					//	REQUIRE(math::det(Matrix3f({ { 4,-1,0 },{ 1,3,2 },{5,3,4} })) == 18);
				}
				SECTION("Inverse") {
					REQUIRE(math::inverse(Matrix2f({ {-1,0},{3,2} })) == Matrix2f({ {-1,0},{1.5f,0.5f} }));

				}
			}
		}

	}

	TEST_CASE("Testing flatten()", "[utility][vector]") {
		SECTION("Vector flatten()") {
			float arr[4];
			Vector4f({ 3.0f,4.0f,1.0f,-1.0f }).flatten(arr);
			float result[] = { 3.0f,4.0f,1.0f,-1.0f };
			for (Index i = 0; i < 4; i++) {
				REQUIRE(arr[i] == result[i]);
			}

		}
		SECTION("Matrix flatten()") {
			SECTION("With square array") {
				float content[4][4] = { { 1,2,3,4 },{ 5,4,3,2 },{ 1,3,2,4 },{ 0,-1,3,10 } };
				const Matrix4f m = Matrix4f(content);
				float arr[16];
				m.flatten(arr);
				Index x = 0, y = 0;
				for (Index i = 0; i < 16; i++) {
					if (y == 4) {
						y = 0;
						x++;
					}
					REQUIRE(m[x][y] == arr[i]);
					y++;
				}

			}
			SECTION("With non-square array") {
				float content[4][3] = { { 1,2,3 },{ 5,4,3 },{ 1,3,2 },{ 0,-1,3 } };
				const Matrix<float, 4, 3> m = Matrix<float, 4, 3>(content);
				float arr[12];
				m.flatten(arr);
				Index x = 0, y = 0;
				for (Index i = 0; i < 12; i++) {
					if (y == 3) {
						y = 0;
						x++;
					}
					REQUIRE(m[x][y] == arr[i]);
					y++;
				}

			}
		}
	}

	TEST_CASE("Testing vector sizes and presets", "[utility][vector]") {
		SECTION("Testing Vector size()") {
			Vector<int, 10> v = Vector<int, 10>();
			REQUIRE(v.size() == 10);

			SECTION("Testing vector float presets") {
				Vector1f v1f = Vector1f();
				REQUIRE(v1f.size() == 1);
				Vector2f v2f = Vector2f();
				REQUIRE(v2f.size() == 2);
				Vector3f v3f = Vector3f();
				REQUIRE(v3f.size() == 3);
				Vector4f v4f = Vector4f();
				REQUIRE(v4f.size() == 4);
				Vector5f v5f = Vector5f();
				REQUIRE(v5f.size() == 5);
			}
			SECTION("Testing vector int presets") {
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
		SECTION("Testing Matrix size(), width(), and height()") {
			Matrix<int, 3, 7> m = Matrix<int, 3, 7>();
			REQUIRE(m.size() == 21);
			REQUIRE(m.width() == 3);
			REQUIRE(m.height() == 7);

			SECTION("Testing Matrix float presets") {
				Matrix1f m1f = Matrix1f();
				REQUIRE(m1f.size() == 1);
				REQUIRE(m1f.width() == 1);
				REQUIRE(m1f.height() == 1);
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
			SECTION("Testing Matrix int presets") {
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
