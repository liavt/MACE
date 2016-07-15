#pragma once
#include <array>
#include "Exceptions.h"
#include "Constants.h"

namespace mc {
	/**
	1-dimensional vector class that supports mathmatical operations.
	
	@tparam T what the `Vector` is made of and calculates with.
	@tparam N width of the `Vector`
	*/
	template <typename T,int N>
	class Vector {
	public:

		std::array < T, N>& getContents()
		{
			return this->content;
		};

		const std::array < T,N>& getContents() const
		{
			return this->content;
		};
		void setContents(const std::array<T,N> contents)
		{
			this->content = contents;
		};

		virtual unsigned int size() const
		{
			return N;
		};


		T& get(unsigned i){
			if (i >= N)throw IndexOutOfBounds(std::to_string(i) + " is greater than the size of this vector, " + std::to_string(N) + "!");
			if (i < 0)throw IndexOutOfBounds(std::to_string(i) + " is less than 0!");
			return content[i];
		}

		const T& get(unsigned int i) const{
			if (i >= N)throw IndexOutOfBounds(std::to_string(i) + " is greater than the size of this vector, " + std::to_string(N) + "!");
			if (i <0)throw IndexOutOfBounds(std::to_string(i) + " is less than 0!");
			return content.at(i);
		}

		void set(unsigned int position, T value) {
			if (position >= N)throw IndexOutOfBounds(std::to_string(position) + " is greater than the size of this vector, " + std::to_string(N) + "!");
			if (position <0)throw IndexOutOfBounds(std::to_string(position) + " is less than 0!");
			content[position] = value;
		}

		T * begin()
		{
			return content.begin();
		};
		T * end()
		{
			return content.end();
		};

		T& operator[](int i)
		{
			return content[i];
		};

		const T& operator[](int i) const
		{
			return content[i];
		};

		Vector operator+(const Vector<T, N>& right) const {
			std::array<T, N> out = std::array<T, N>();
			for (unsigned int i = 0; i < N; i++) {
				out[i]=((T)(this->get(i) + right.get(i)));
			}
			return mc::Vector<T, N>(out);
		};

		Vector operator-(const Vector<T, N>& right) const {
			std::array<T, N> out = std::array<T, N>();
			for (unsigned int i = 0; i < N; i++) {
				out[i] = ((T)(this->get(i) - right.get(i)));
			}
			return mc::Vector<T, N>(out);
		};

		Vector operator/(const Vector<T, N>& right) const {
			std::array<T, N> out = std::array<T, N>();
			for (unsigned int i = 0; i < N; i++) {
				out[i] = ((T)(this->get(i) / right.get(i)));
			}
			return mc::Vector<T, N>(out);
		};

		Vector operator*(const Vector<T, N>& right) const {
			std::array<T, N> out = std::array<T, N>();
			for (unsigned int i = 0; i < N; i++) {
				out[i] = ((T)(this->get(i) * right.get(i)));
			}
			return mc::Vector<T, N>(out);
		};


		template<typename TOther, int NOther>
		bool operator==(const Vector<TOther, NOther>& other)
		{
			if (N != NOther)return false;
			if (T != TOther)return false;
			for (int i = 0; i < N; i++) {
				if (this[i] != other[i]) {
					return false;
				}
			}
			return true;
		};

		template<typename TOther, int NOther>
		bool operator!=(const Vector<TOther, NOther>& other)
		{
			return !(this == other);
		};

		Vector operator=(T arr[N]) {//arr me mateys
			return Vector(arr);
		}

		Vector(T arr[N]) {
			this->setContents(std::array<T,N>(arr));
		}

		Vector()
		{
			this->setContents(std::array<T, N>());
		};
		Vector(std::array<T,N>& contents)
		{
			this->setContents(contents);
		};
		Vector(const Vector &obj)
		{
			this->setContents(obj.getContents());
		};

	protected:
		std::array<T,N> content;
	};



	using Vector1f = mc::Vector<float, 1>;
	using Vector2f = mc::Vector<float, 2>;
	using Vector3f = mc::Vector<float, 3>;
	using Vector4f = mc::Vector<float, 4>;
	using Vector5f = mc::Vector<float, 5>;

	using Vector1i = mc::Vector<int, 1>;
	using Vector2i = mc::Vector<int, 2>;
	using Vector3i = mc::Vector<int, 3>;
	using Vector4i = mc::Vector<int, 4>;
	using Vector5i = mc::Vector<int, 5>;

	/**
	Used in the `Matrix` class.
	@tparam T What data type the row is made of
	@tparam N The width of the row
	*/
	template <typename T, int N>
	using MatrixRow = mc::Vector<T, N>;//this is for clarity

	/*
	So why are there also presets for MatrixRows when there are presets for Vectors? Here's the reason:

	Lets say I am iterating over a Matrix via the enchanced for loop. With these presets, I could do this:

	Matrix5f matrix = Matrix5f();

	for(MatrixRow5f row : matrix){
		for(float value : row){
			--do stuff
		}
	}

	Without these presets, the above would have to be done like this:

	for(MatrixRow<float, 5> row : matrix){
		for(float value : row){
			--do stuff
		}
	}

	or

	for(Vector<float, 5> row : matrix){
		for(float value : row){
			--do stuff
		}
	}

	So whats the point of a Matrix typedef if the client has to use templates? The entire point of these giant lists of typedefs is for the user to NOT use templates.
	The templates are there for more advanced user, but for a beginner, MatrixRow5f looks a lot better and easier than MatrixRow<float,5>
	*/
	using MatrixRow1f = mc::MatrixRow<float, 1>;
	using MatrixRow2f = mc::MatrixRow<float, 2>;
	using MatrixRow3f = mc::MatrixRow<float, 3>;
	using MatrixRow4f = mc::MatrixRow<float, 4>;
	using MatrixRow5f = mc::MatrixRow<float, 5>;

	using MatrixRow1i = mc::MatrixRow<int, 1>;
	using MatrixRow2i = mc::MatrixRow<int, 2>;
	using MatrixRow3i = mc::MatrixRow<int, 3>;
	using MatrixRow4i = mc::MatrixRow<int, 4>;
	using MatrixRow5i = mc::MatrixRow<int, 5>;

	/**
	A class representing a 2-dimensional matrix, and allows for math involving matrices.
	@tparam T What the `Matrix should consist of
	@tparam W The width of the `Matrix`
	@tparam H The height of the `Matrix`
	*/
	template<typename T, int W, int H>
	struct Matrix : Vector<MatrixRow<T, H>, W> {
		using Vector::Vector;

		Matrix()
		{
			this->setContents(std::array<MatrixRow<T, H>, W>());
			for (unsigned int i = 0; i < content.size(); i++) {
				content[i] = MatrixRow<T, H>();
			}
		};

		Matrix(T arr[W][H]) {
			Matrix();
			for (int x = 0; x < sizeof(arr) / sizeof(T); x++) {
				for (int y = 0; y < sizeof(arr[x]) / sizeof(T); y++) {
					content[x][y] = arr[x][y];
				}
			}
		}

		Matrix(const Matrix& copy) {
			this->setContents(copy.getContents());
		}

		Size size() {
			return W*H;
		}

		Size width() {
			return W;
		}
		Size height() {
			return H;
		}

		T& get(unsigned int x, unsigned int y) {
			if (x >= W) throw IndexOutOfBounds(std::to_string(x)+ " is greater than this Matrix's width, " + std::to_string(W) + "!");
			if (y >= H) throw IndexOutOfBounds(std::to_string(y) + " is greater than this Matrix's width, " + std::to_string(H) + "!");
			if (x <0)throw IndexOutOfBounds("The X value, "+std::to_string(x) + " is less than 0!");
			if (y <0)throw IndexOutOfBounds("The Y value, " + std::to_string(x) + " is less than 0!");
			return content[x][y];
		}

		const T& get(unsigned int x, unsigned int y) const {
			if (x >= W) throw IndexOutOfBounds(std::to_string(x) + " is greater than this Matrix's width, " + std::to_string(W) + "!");
			if (y >= H) throw IndexOutOfBounds(std::to_string(y) + " is greater than this Matrix's width, " + std::to_string(H) + "!");
			if (x <0)throw IndexOutOfBounds("The X value, " + std::to_string(x) + " is less than 0!");
			if (y <0)throw IndexOutOfBounds("The Y value, " + std::to_string(x) + " is less than 0!");
			return content[x][y];
		}

		void set(unsigned int x, unsigned int y, T value) {
			if (x >= W) throw IndexOutOfBounds(std::to_string(x)+" is greater than this Matrix's width, "+ std::to_string(W)+"!");
			if (y >= H) throw IndexOutOfBounds(std::to_string(y) + " is greater than this Matrix's width, " + std::to_string(H) + "!");
			if (x <0)throw IndexOutOfBounds("The X value, " + std::to_string(x) + " is less than 0!");
			if (y <0)throw IndexOutOfBounds("The Y value, " + std::to_string(x) + " is less than 0!");
			content[x][y] = value;
		}
	};

	using Matrix1f = mc::Matrix<float, 1, 1>;//what a thin matrix!
	using Matrix2f = mc::Matrix<float, 2, 2>;
	using Matrix3f = mc::Matrix<float, 3, 3>;
	using Matrix4f = mc::Matrix<float, 4, 4>;
	using Matrix5f = mc::Matrix<float, 5, 5>;

	using Matrix1i = mc::Matrix<int, 1, 1>;
	using Matrix2i = mc::Matrix<int, 2, 2>;
	using Matrix3i = mc::Matrix<int, 3, 3>;
	using Matrix4i = mc::Matrix<int, 4, 4>;
	using Matrix5i = mc::Matrix<int, 5, 5>;
}