/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <catch2/catch.hpp>
#include <MACE/Utility/Matrix.h>

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
			for (int x = 1; x <= 5; x++) {
				REQUIRE_NOTHROW(m(x, 1));
				m(x, 1) = x;
				REQUIRE(m(x, 1) == x);
			}
		}

		SECTION("Testing if the [] operator works") {
			for (int x = 0; x < 5; x++) {
				REQUIRE_NOTHROW(m[x][0]);
				m[x][0] = x;
				REQUIRE(m[x][0] == x);
			}
		}
	}

	TEST_CASE("Testing operators", "[utility][vector]") {
		SECTION("Vector operators") {
			SECTION("Equality and inequality") {
				Vector<int, 4> v1 = { 1,4,3,5 };
				Vector<int, 4> v2 = { 3,4,6,3 };
				REQUIRE_FALSE(v1 == v2);
				REQUIRE(v1 != v2);
				v1 = v2;
				REQUIRE(v1 == v2);
				REQUIRE_FALSE(v1 != v2);
			}
			SECTION("Greater than and less than") {
				Vector<int, 4> v1 = { 1,4,3,5 };
				Vector<int, 4> v2 = { 3,5,6,6 };
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
				Matrix<int, 3> m1 = { {1,3,4},{5,4,6},{4,3,4} };
				Matrix<int, 3> m2 = { { 4,5,3},{ 2,1,6 },{ 3,1,-1 } };
				REQUIRE_FALSE(m1 == m2);
				REQUIRE(m1 != m2);
				m1 = m2;
				REQUIRE(m1 == m2);
				REQUIRE_FALSE(m1 != m2);
			}
			SECTION("Greater than and less than") {
				Matrix<int, 3> m1 = { { 1,3,4 },{ 5,4,6 },{ 4,3,4 } };
				Matrix<int, 3> m2 = { { 4,5,6 },{ 6,5,8 },{ 7,5,6 } };
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
				Vector<float, 3> v = arr;
				REQUIRE(v[0] == 1.0f);
				REQUIRE(v[1] == 42.0f);
				REQUIRE(v[2] == 100.0f);
			}
			SECTION("Testing single value constructor") {
				REQUIRE(Vector<float, 3>(2.0f) == Vector<float, 3>({ 2.0f, 2.0f, 2.0f }));
				REQUIRE(Vector<int, 2>(1) == Vector<int, 2>({ 1, 1 }));
			}
			SECTION("Testing default constructor") {
				Vector<float, 3> v = Vector<float, 3>();
				REQUIRE(v[0] == 0);
				REQUIRE(v[1] == 0);
				REQUIRE(v[2] == 0);
			}
			SECTION("Testing contructor with array") {
				float arr[] = { 45.512f,6.9f,4.20f };
				Vector<float, 3> v = Vector<float, 3>(arr);
				REQUIRE(v.get(0) == 45.512f);
				REQUIRE(v.get(1) == 6.9f);
				REQUIRE(v.get(2) == 4.20f);
			}
			SECTION("Testing contructor with std::array") {
				std::array<float, 3> arr = { -41.7f,100.9f,1.1f };
				Vector<float, 3> v = Vector<float, 3>(arr);
				REQUIRE(v.get(0) == (-41.7f));
				REQUIRE(v.get(1) == 100.9f);
				REQUIRE(v.get(2) == 1.1f);
			}
			SECTION("Testing aggregate constructor") {
				SECTION("Constructor-stlye") {
					Vector<int, 4> v = Vector<int, 4>({ 4,1,32,5 });
					REQUIRE(v[0] == 4);
					REQUIRE(v[1] == 1);
					REQUIRE(v[2] == 32);
					REQUIRE(v[3] == 5);
				}
				SECTION("Aggregate-style") {
					Vector<int, 4> v = { 4,1,32,5 };
					REQUIRE(v[0] == 4);
					REQUIRE(v[1] == 1);
					REQUIRE(v[2] == 32);
					REQUIRE(v[3] == 5);
				}
			}
			SECTION("Testing copy constructors") {
				Vector<float, 3> v = Vector<float, 3>();
				v.set(0, 67.9f);
				v.set(1, 56.0f);
				v.set(2, 42.4f);
				SECTION("Testing Vector(Vector&)") {
					Vector<float, 3> v2 = Vector<float, 3>(v);
					REQUIRE(v.get(0) == 67.9f);
					REQUIRE(v.get(1) == 56.0f);
					REQUIRE(v.get(2) == 42.4f);
				}
				SECTION("Testing copy assignment") {
					Vector<float, 3> v3 = v;//no constructor, just assignment. compiler should use the copy contructor
					REQUIRE(v[0] == 67.9f);
					REQUIRE(v[1] == 56.0f);
					REQUIRE(v[2] == 42.4f);
				}
			}

			SECTION("Type conversion") {
				Vector<int, 1> v1 = { 2 };
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
				Matrix<int, 3> m = arr;
				Index counter = 0;
				for (Index x = 0; x < m.width(); x++) {
					for (Index y = 0; y < m.height(); y++) {
						counter++;
						REQUIRE(m[x][y] == counter);
					}
				}
			}
			SECTION("Testing default constructor") {
				Matrix<float, 2> m = Matrix<float, 2>();
				REQUIRE(m[0][0] == 0);
				REQUIRE(m[0][1] == 0);
				REQUIRE(m[1][0] == 0);
				REQUIRE(m[1][1] == 0);
			}
			SECTION("Testing constructor with array") {
				int arr[3][3] = { { 1,2,3 },{ 4,5,6 },{ 7,8,9 } };
				Matrix<int, 3> m = Matrix<int, 3>(arr);
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
					const Matrix<int, 3> m = Matrix<int, 3>({ { 1,2,3 },{ 1,2,3 },{ 1,2,3 } });
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
					const Matrix<int, 3> m = { {1,2,3}, {1,2,3}, {1,2,3} };
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
				Matrix<int, 4> m = Matrix<int, 4>();
				for (int x = 0; x < m.width(); ++x) {
					for (int y = 0; y < m.height(); ++y) {
						m[x][y] = x * y;
					}
				}
				SECTION("Testing via explicit constructor") {
					Matrix<int, 4> other = Matrix<int, 4>(m);
					for (int x = 0; x < other.width(); ++x) {
						for (int y = 0; y < other.height(); ++y) {
							REQUIRE(other[x][y] == x * y);
						}
					}
				}
				SECTION("Testing via assignment") {
					Matrix<int, 4> other = m;
					for (int x = 0; x < other.width(); ++x) {
						for (int y = 0; y < other.height(); ++y) {
							REQUIRE(other[x][y] == x * y);
						}
					}
				}
			}
		}
	}

	TEST_CASE("Testing getting and setting", "[utility][vector]") {
		SECTION("Testing getting and setting of a vector") {
			Vector<int, 5> v = Vector<int, 5>();

			for (int i = 0; i < v.size(); ++i) {
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
			Vector<int, 4> v = { 1,2,3,4 };
			REQUIRE(static_cast<bool>(v.x() == Vector<int, 1>({ 1 })));
			REQUIRE(static_cast<bool>(v.y() == Vector<int, 1>({ 2 })));
			REQUIRE(static_cast<bool>(v.z() == Vector<int, 1>({ 3 })));
			REQUIRE(static_cast<bool>(v.w() == Vector<int, 1>({ 4 })));
		}

		SECTION("Testing getting and setting of matrices") {
			Matrix<int, 5> m = Matrix<int, 5>();

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
				Vector<int, 3> v1 = { 1,0,3 };
				Vector<int, 3> v2 = { -1,4,2 };
				Vector<int, 3> result = { 0,4,5 };
				REQUIRE(v1 + v2 == result);
			}
			SECTION("Subtracting") {
				Vector<int, 3> v1 = { 1,0,3 };
				Vector<int, 3> v2 = { -1,4,2 };
				Vector<int, 3> result = { 2,-4,1 };
				REQUIRE(v1 - v2 == result);
			}
			SECTION("Multiplying") {
				Vector<int, 3> v1 = { 1,0,3 };
				Vector<int, 3> v2 = { -1,4,2 };
				Vector<int, 3> result = { -1,0,6 };
				REQUIRE(v1*v2 == result);
			}
			SECTION("Dot product") {
				Vector<int, 3> v1 = { 1,0,3 };
				Vector<int, 3> v2 = { -1,4,2 };
				REQUIRE(math::dot(v1, v2) == 5);
			}
			SECTION("Cross product") {
				Vector<int, 3> v1 = { 1,0,3 };
				Vector<int, 3> v2 = { -1,4,2 };
				REQUIRE(math::cross(v1, v2) == Vector<int, 3>({ -12,-5,4 }));
			}
			SECTION("Magnitudes") {
				Vector<float, 3> v1 = { 1,0,3 };
				Vector<float, 3> v2 = { -1,4,2 };
				REQUIRE(math::magnitude(v1) == Approx(3.162277));
				REQUIRE(math::magnitude(v2) == Approx(4.582575));
			}
			SECTION("Normalizing") {
				Vector<float, 3> input = { 3,-4,0 };
				Vector<float, 3> output = { (3.0f / 5.0f),-(4.0f / 5.0f),0 };
				REQUIRE(math::normalize(input) == output);
			}
			SECTION("Scalar multiplication") {
				REQUIRE(Vector<float, 3>({ 5,-9,11 }) * 1 == Vector<float, 3>({ 5,-9,11 }));
				REQUIRE(Vector<float, 3>({ 5,-9,11 }) * 2 == Vector<float, 3>({ 10,-18,22 }));
				REQUIRE(Vector<float, 3>({ 5,-9,11 }) * -2 == Vector<float, 3>({ -10,18,-22 }));
			}
		}

		SECTION("Matrix math") {
			SECTION("Matrix by Matrix") {
				SECTION("Adding") {
					REQUIRE(Matrix<int, 3>({ {1,2,3},{4,5,6},{7,8,9} }) + Matrix<int, 3>({ {3,3,12},{4,5,6},{3,7,8} }) == Matrix<int, 3>({ {4,5,15},{8,10,12},{10,15,17} }));
				}
				SECTION("Subtracting") {
					REQUIRE(Matrix<int, 3>({ { 1,2,3 },{ 4,5,6 },{ 7,8,9 } }) - Matrix<int, 3>({ { 3,3,12 },{ 4,5,6 },{ 3,7,8 } }) == Matrix<int, 3>({ { -2,-1,-9 },{ 0,0,0 },{ 4,1,1 } }));

				}
				SECTION("Multiplying") {
					REQUIRE(Matrix<int, 3>({ { 1,2,3 },{ 4,5,6 },{ 7,8,9 } }) * Matrix<int, 3>({ { 3,3,12 },{ 4,5,6 },{ 3,7,8 } }) == Matrix<int, 3>({ { 20,34,48 },{ 50,79,126 },{ 80,124,204 } }));
				}
			}
			SECTION("Matrix by Vector") {
				SECTION("Multiplying") {
					REQUIRE(Matrix<int, 3>({ { 1,3,2 },{ 4,-1,5 },{ 0,6,7 } }) * Vector<int, 3>({ 3,-2,0 }) == Vector<int, 3>({ -5,2,0 }));
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
					REQUIRE(math::det(Matrix<float, 2>({ {1,3},{5,4} })) == -11);
					//	REQUIRE(math::det(Matrix<float, 3>({ { 4,-1,0 },{ 1,3,2 },{5,3,4} })) == 18);
				}
				SECTION("Inverse") {
					REQUIRE(math::inverse(Matrix<float, 2>({ {-1,0},{3,2} })) == Matrix<float, 2>({ {-1,0},{1.5f,0.5f} }));

				}
			}
		}
	}

	TEST_CASE("Testing lerp() on Vectors", "[utility][vector]") {
		REQUIRE(math::lerp(Vector<int, 3>({ 0, 0, 0 }), Vector<int, 3>({ 1,1,1 }), 0) == Vector<int, 3>({ 0,0,0 }));
		REQUIRE(math::lerp(Vector<int, 3>({ 0, 0, 0 }), Vector<int, 3>({ 1,1,1 }), 1) == Vector<int, 3>({ 1,1,1 }));
		REQUIRE(math::lerp(Vector<float, 3>({ 1.0f, 1.0f, 1.0f }), Vector<float, 3>({ 1.0f,1.0f,1.0f }), 0.5f) == Vector<float, 3>({ 1.0f,1.0f,1.0f }));
		REQUIRE(math::lerp(Vector<float, 3>({ 0.0f, 0.0f, 0.0f }), Vector<float, 3>({ 10.0f,10.0f,10.0f }), 0.5f) == Vector<float, 3>({ 5.0f,5.0f,5.0f }));
		REQUIRE(math::lerp(Vector<double, 3>({ 0.0, 0.0, 0.0 }), Vector<double, 3>({ 10.0,10.0,10.0 }), 0.3) == Vector<double, 3>({ 3.0,3.0,3.0 }));
		REQUIRE(math::lerp(Vector<double, 3>({ 0.0, 0.0, 0.0 }), Vector<double, 3>({ 10.0,10.0,10.0 }), 0.71) == Vector<double, 3>({ 7.1,7.1,7.1 }));
	}

	TEST_CASE("Testing flatten()", "[utility][vector]") {
		SECTION("Vector flatten()") {
			float arr[4];
			Vector<float, 4>({ 3.0f,4.0f,1.0f,-1.0f }).flatten(arr);
			float result[] = { 3.0f,4.0f,1.0f,-1.0f };
			for (Index i = 0; i < 4; i++) {
				REQUIRE(arr[i] == result[i]);
			}

		}
		SECTION("Matrix flatten()") {
			SECTION("With square array") {
				float content[4][4] = { { 1,2,3,4 },{ 5,4,3,2 },{ 1,3,2,4 },{ 0,-1,3,10 } };
				const Matrix<float, 4> m = Matrix<float, 4>(content);
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
				Vector<float, 1> v1f = Vector<float, 1>();
				REQUIRE(v1f.size() == 1);
				Vector<float, 2> v2f = Vector<float, 2>();
				REQUIRE(v2f.size() == 2);
				Vector<float, 3> v3f = Vector<float, 3>();
				REQUIRE(v3f.size() == 3);
				Vector<float, 4> v4f = Vector<float, 4>();
				REQUIRE(v4f.size() == 4);
				Vector<float, 5> v5f = Vector<float, 5>();
				REQUIRE(v5f.size() == 5);
			}
			SECTION("Testing vector int presets") {
				Vector<int, 1> v1i = Vector<int, 1>();
				REQUIRE(v1i.size() == 1);
				Vector<int, 2> v2i = Vector<int, 2>();
				REQUIRE(v2i.size() == 2);
				Vector<int, 3> v3i = Vector<int, 3>();
				REQUIRE(v3i.size() == 3);
				Vector<int, 4> v4i = Vector<int, 4>();
				REQUIRE(v4i.size() == 4);
				Vector<int, 5> v5i = Vector<int, 5>();
				REQUIRE(v5i.size() == 5);
			}
		}
		SECTION("Testing Matrix size(), width(), and height()") {
			Matrix<int, 3, 7> m = Matrix<int, 3, 7>();
			REQUIRE(m.size() == 21);
			REQUIRE(m.width() == 3);
			REQUIRE(m.height() == 7);

			SECTION("Testing Matrix float presets") {
				Matrix<float, 1> m1f = Matrix<float, 1>();
				REQUIRE(m1f.size() == 1);
				REQUIRE(m1f.width() == 1);
				REQUIRE(m1f.height() == 1);
				Matrix<float, 2> m2f = Matrix<float, 2>();
				REQUIRE(m2f.size() == 4);
				REQUIRE(m2f.width() == 2);
				REQUIRE(m2f.height() == 2);
				Matrix<float, 3> m3f = Matrix<float, 3>();
				REQUIRE(m3f.size() == 9);
				REQUIRE(m3f.width() == 3);
				REQUIRE(m3f.height() == 3);
				Matrix<float, 4> m4f = Matrix<float, 4>();
				REQUIRE(m4f.size() == 16);
				REQUIRE(m4f.width() == 4);
				REQUIRE(m4f.height() == 4);
				Matrix<float, 5> m5f = Matrix<float, 5>();
				REQUIRE(m5f.size() == 25);
				REQUIRE(m5f.width() == 5);
				REQUIRE(m5f.height() == 5);
			}
			SECTION("Testing Matrix int presets") {
				Matrix<int, 1> m1i = Matrix<int, 1>();
				REQUIRE(m1i.size() == 1);
				REQUIRE(m1i.width() == 1);
				REQUIRE(m1i.height() == 1);
				Matrix<int, 2> m2i = Matrix<int, 2>();
				REQUIRE(m2i.size() == 4);
				REQUIRE(m2i.width() == 2);
				REQUIRE(m2i.height() == 2);
				Matrix<int, 3> m3i = Matrix<int, 3>();
				REQUIRE(m3i.size() == 9);
				REQUIRE(m3i.width() == 3);
				REQUIRE(m3i.height() == 3);
				Matrix<int, 4> m4i = Matrix<int, 4>();
				REQUIRE(m4i.size() == 16);
				REQUIRE(m4i.width() == 4);
				REQUIRE(m4i.height() == 4);
				Matrix<int, 5> m5i = Matrix<int, 5>();
				REQUIRE(m5i.size() == 25);
				REQUIRE(m5i.width() == 5);
				REQUIRE(m5i.height() == 5);
			}
		}
	}
}
