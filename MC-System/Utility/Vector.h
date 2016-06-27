#pragma once
#include <array>

namespace mc {
	template <class T,int N>
	class Vector {
	public:

		std::array < T,N>* getContents() const;
		void setContents(std::array<T,N> contents);

		int * size();

		T * begin();
		T * end();

		T& operator[](int i);
		Vector operator*(const Vector& m) const;
		Vector operator*=(const Vector& m);
		Vector operator/(const Vector& m) const;
		Vector operator/=(const Vector& m);
		Vector operator+(const Vector& m) const;
		Vector operator+=(const Vector& m);
		Vector operator-(const Vector& m) const;
		Vector operator-=(const Vector& m);

		bool operator==(const Vector& other);
		bool operator!=(const Vector& other);


		Vector();
		Vector(std::array<T,N>& contents);
		Vector(const Vector &obj);

	protected:
		std::array<T,N> content;
	};
	
	using Vector1f = mc::Vector<float,1>  ;
	using Vector2f = mc::Vector<float, 2>;
	using Vector3f = mc::Vector<float, 3>;
	using Vector4f = mc::Vector<float, 4>;
	using Vector5f = mc::Vector<float, 5>;

	using Vector1i = mc::Vector<int, 1>;
	using Vector2i = mc::Vector<int, 2>;
	using Vector3i = mc::Vector<int, 3>;
	using Vector4i = mc::Vector<int, 4>;
	using Vector5i = mc::Vector<int, 5>;

	template <class T, int N>
	using MatrixColumn = mc::Vector<T, N>;//this is for clartiy

	template <class T, int W, int H>
	using Matrix = mc::Vector<mc::MatrixColumn<T, H>, W>;

	using Matrix1f = mc::Matrix<float, 1,1>;
	using Matrix2f = mc::Matrix<float, 2,2>;
	using Matrix3f = mc::Matrix<float, 3,3>;
	using Matrix4f = mc::Matrix<float, 4,4>;
	using Matrix5f = mc::Matrix<float, 5,5>;

	using Matrix1i = mc::Matrix<int, 1,1>;
	using Matrix2i = mc::Matrix<int, 2,2>;
	using Matrix3i = mc::Matrix<int, 3,3>;
	using Matrix4i = mc::Matrix<int, 4,4>;
	using Matrix5i = mc::Matrix<int, 5,5>;



}