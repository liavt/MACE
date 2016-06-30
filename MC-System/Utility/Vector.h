#pragma once
#include <array>

namespace mc {
	/**
	@brief Class that allows for vector math
	
	@tparam T what the {@code Vector} is made of and calculates with.
	@tparam N width of the {@code Vector}
	*/
	template <typename T,int N>
	class Vector {
	public:

		std::array < T,N>* getContents() const
		{
			return this->content;
		};
		void setContents(std::array<T,N> contents)
		{
			this->content = contents;
		};

		unsigned int size() const
		{
			return N;
		};


		T& get(int i){
			return const_cast<T&>(static_cast<const std::array<T, N>>(content)->get(i));;
		}

		const T& get(int i) const{
			return content.at(i);
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
			return const_cast<T&>(static_cast<const std::array<T,N>>(content)[i]);;
		};

		const T& operator[](int i) const
		{
			return content[i];
		};

		Vector operator+(const Vector<T, N>& right) const {
			std::array<T, N> out = std::array<T, N>();
			for (unsigned int i = 0; i < N; i++) {
				out[i]=((T)(&this[i] + right[i]));
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
			this->setContents(obj->getContents());
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

	template <typename T, int N>
	using MatrixColumn = mc::Vector<T, N>;//this is for clarity

	template <typename T, int W, int H>
	using Matrix = mc::Vector<mc::MatrixColumn<T, W>, H>;

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