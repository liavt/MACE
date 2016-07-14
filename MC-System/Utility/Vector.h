#pragma once
#include <array>
#include <MC-System/Exceptions.h>
#include <MC-System/Constants.h>

namespace mc {
	template<typename T, int W, int H>
	struct Matrix;//forward-defined here for the friend declaration
	/**
	1-dimensional vector class that supports mathmatical operations.
	
	@tparam T what the `Vector` is made of and calculates with.
	@tparam N width of the `Vector`
	*/
	template <typename T,int N>
	class Vector {
		/**
		`Matrix` is friends with `Vector` so `Matrix` can create efficient implementations of `get()` and `set()`
		*/
		template<typename T, int W, int H>
		friend struct Matrix;
	public:
		Vector()
		{
			this->setContents(std::array<T, N>());
		};

		Vector(T arr[N]) {
			this->setContents(std::array<T, N>(arr));
		}

		Vector(std::array<T, N>& contents)
		{
			this->setContents(contents);
		};
		Vector(const Vector &obj)
		{
			this->setContents(obj.getContents());
		};

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

	private:
		std::array<T,N> content;
	};


	/**
	A `Vector` consisting of 1 `float`
	*/
	using Vector1f = mc::Vector<float, 1>;
	/**
	A `Vector` consisting of 2 `floats`
	*/
	using Vector2f = mc::Vector<float, 2>;
	/**
	A `Vector` consisting of 3 `floats`
	*/
	using Vector3f = mc::Vector<float, 3>;
	/**
	A `Vector` consisting of 4 `floats`
	*/
	using Vector4f = mc::Vector<float, 4>;
	/**
	A `Vector` consisting of 5 `floats`
	*/
	using Vector5f = mc::Vector<float, 5>;

	/**
	A `Vector` consisting of 1 `int`
	*/
	using Vector1i = mc::Vector<int, 1>;
	/**
	A `Vector` consisting of 2 `ints`
	*/
	using Vector2i = mc::Vector<int, 2>;
	/**
	A `Vector` consisting of 3 `ints`
	*/
	using Vector3i = mc::Vector<int, 3>;
	/**
	A `Vector` consisting of 4 `ints`
	*/
	using Vector4i = mc::Vector<int, 4>;
	/**
	A `Vector` consisting of 5 `ints`
	*/
	using Vector5i = mc::Vector<int, 5>;

	/**
	Used in the `Matrix` class.
	@tparam T What data type the row is made of
	@tparam N The width of the row
	*/
	template <typename T, int N>
	using MatrixRow = mc::Vector<T, N>;//this is for clarity

	/**
	A class representing a 2-dimensional matrix, and allows for math involving matrices.
	@tparam T What the `Matrix should consist of
	@tparam W The width of the `Matrix`
	@tparam H The height of the `Matrix`
	*/
	template<typename T, int W, int H>
	struct Matrix : Vector<MatrixRow<T, H>, W> {
		using Vector::Vector;

		/**
		Default constructor. Creates a `Matrix` of the specified size where every spot is empty.
		*/
		Matrix()
		{
			this->setContents(std::array<MatrixRow<T, H>, W>());
			for (unsigned int i = 0; i < content.size(); i++) {
				content[i] = MatrixRow<T, H>();
			}
		};

		/**
		Creates a `Matrix` based on a 2-dimensional array. The array's contents will be copied into this `Matrix`
		@param arr An array of contents
		*/
		Matrix(T arr[W][H]) {
			Matrix();
			for (int x = 0; x < sizeof(arr) / sizeof(T); x++) {
				for (int y = 0; y < sizeof(arr[x]) / sizeof(T); y++) {
					content[x][y] = arr[x][y];
				}
			}
		}

		/**
		Copy constructor. Clones the contents of another `Matrix` into a new `Matrix`
		@param copy What the clone
		*/
		Matrix(const Matrix& copy) {
			this->setContents(copy.getContents());
		}

		/**
		Calculates how many elements are in this `Matrix`
		@return `width()` times `height()`
		@see #height()
		@see #width()
		*/
		Size size() {
			return W*H;
		}

		/**
		Calculates the width of this `Matrix`.
		@return The width specified by the template.
		@see #height()
		@see #size()
		*/
		Size width() {
			return W;
		}
		/**
		Calculates the height of this `Matrix`.
		@return The height specified by the template.
		@see #width()
		@see #size()
		*/
		Size height() {
			return H;
		}

		/**
		Retrieves the content at a certain position
		@param x X-coordinate of the requested data
		@param y Y-coordinate of the requested data
		@return A reference to the data at `X,Y`
		@throw IndexOutOfBounds if `x<0`
		@throw IndexOutOfBounds if `y<0`
		@throw IndexOutOfBounds if `x>width()`
		@throw IndexOutOfBounds if `y>height()`
		@see #set(unsigned int, unsigned int)
		@see #operator[]
		*/
		T& get(unsigned int x, unsigned int y) {
			if (x >= W) throw IndexOutOfBounds(std::to_string(x) + " is greater than this Matrix's width, " + std::to_string(W) + "!");
			if (y >= H) throw IndexOutOfBounds(std::to_string(y) + " is greater than this Matrix's width, " + std::to_string(H) + "!");
			if (x < 0)throw IndexOutOfBounds("The X value, " + std::to_string(x) + " is less than 0!");
			if (y < 0)throw IndexOutOfBounds("The Y value, " + std::to_string(x) + " is less than 0!");
			return content[x][y];
		}

		/**
		`const` version of {@link #get(unsigned int, unsigned int)}
		@param x X-coordinate of the requested data
		@param y Y-coordinate of the requested data
		@return A reference to the `const` data at `X,Y`
		@throw IndexOutOfBounds if `x<0`
		@throw IndexOutOfBounds if `y<0`
		@throw IndexOutOfBounds if `x>width()`
		@throw IndexOutOfBounds if `y>height()`
		@see #set(unsigned int, unsigned int)
		@see #operator[] const
		*/
		const T& get(unsigned int x, unsigned int y) const {
			if (x >= W) throw IndexOutOfBounds(std::to_string(x) + " is greater than this Matrix's width, " + std::to_string(W) + "!");
			if (y >= H) throw IndexOutOfBounds(std::to_string(y) + " is greater than this Matrix's width, " + std::to_string(H) + "!");
			if (x < 0)throw IndexOutOfBounds("The X value, " + std::to_string(x) + " is less than 0!");
			if (y < 0)throw IndexOutOfBounds("The Y value, " + std::to_string(x) + " is less than 0!");
			return content[x][y];
		}


		/**
		Writes data at certain coordinates to a new value.
		@param x X-coordinate of the new data
		@param y Y-coordinate of the new data
		@param value New datat to write to `X,Y`
		@throw IndexOutOfBounds if `x<0`
		@throw IndexOutOfBounds if `y<0`
		@throw IndexOutOfBounds if `x>width()`
		@throw IndexOutOfBounds if `y>height()`
		@see #operator[]
		@see #get(unsigned int, unsigned int)
		*/
		void set(unsigned int x, unsigned int y, T value) {
			if (x >= W) throw IndexOutOfBounds(std::to_string(x) + " is greater than this Matrix's width, " + std::to_string(W) + "!");
			if (y >= H) throw IndexOutOfBounds(std::to_string(y) + " is greater than this Matrix's width, " + std::to_string(H) + "!");
			if (x < 0)throw IndexOutOfBounds("The X value, " + std::to_string(x) + " is less than 0!");
			if (y < 0)throw IndexOutOfBounds("The Y value, " + std::to_string(x) + " is less than 0!");
			content[x][y] = value;
		}

		/**
		Assignment operator that allows for aggregate-like initialization.
		<p>
		Equal to calling: 
		{@code
			Matrix(arr)
		}
		@param arr 2-dimensional array of data
		@return A `Matrix` containing the contents of the array.
		*/
		Matrix operator=(T arr[W][H]) {//arr me mateys
			return Matrix(arr);
		}
	};

	/**
	A `1x1` `Matrix` of `floats`.
	*/
	using Matrix1f = mc::Matrix<float, 1, 1>;//what a thin matrix
	/**
	A `2x2` `Matrix` of `floats`.
	*/
	using Matrix2f = mc::Matrix<float, 2, 2>;
	/**
	A `3x3` `Matrix` of `floats`.
	*/
	using Matrix3f = mc::Matrix<float, 3, 3>;
	/**
	A `4x4` `Matrix` of `floats`.
	*/
	using Matrix4f = mc::Matrix<float, 4, 4>;
	/**
	A `5x5` `Matrix` of `floats`.
	*/
	using Matrix5f = mc::Matrix<float, 5, 5>;

	/**
	A `1x1` `Matrix` of `ints`.
	*/
	using Matrix1i = mc::Matrix<int, 1, 1>;
	/**
	A `2x2` `Matrix` of `ints`.
	*/
	using Matrix2i = mc::Matrix<int, 2, 2>;
	/**
	A `3x3` `Matrix` of `ints`.
	*/
	using Matrix3i = mc::Matrix<int, 3, 3>;
	/**
	A `4x4` `Matrix` of `ints`.
	*/
	using Matrix4i = mc::Matrix<int, 4, 4>;
	/**
	A `5x5` `Matrix` of `ints`.
	*/
	using Matrix5i = mc::Matrix<int, 5, 5>;
}