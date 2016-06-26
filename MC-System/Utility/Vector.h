#pragma once
#include <vector>

namespace mc {
	template <class T>
	class Vector {
	public:

		std::vector < T>* getContents() const;
		void setContents(std::vector<T> contents);

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
		Vector(std::vector < T>* contents);

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