#pragma once
#include <vector>

namespace mc {
	template <class T>
	class Vector {
	public:

		std::vector < T> getContents() const;
		void setContents(std::vector<T> contents);

		T& operator[](int i);
		Vector& operator*(Vector* m);
		Vector& operator*=(Vector* m);
		Vector& operator/(Vector* m);
		Vector& operator/=(Vector* m);

		Vector();
		Vector(std::vector < T>);

	protected:
		std::vector < T> content;
	};

	typedef mc::Vector<int> IntVector;
	typedef mc::Vector<float> FloatVector;
	
	template <class T>
	using Matrix= mc::Vector<mc::Vector<T>>;

	typedef mc::Matrix<int> IntMatrix;
	typedef mc::Matrix<float> FloatMatrix;
	
}