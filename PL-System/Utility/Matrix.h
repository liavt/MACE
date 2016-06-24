#pragma once
#include <vector>

namespace mc {
	class Matrix {
	public:
		std::vector < std::vector<float>> getContents() const;
		void setContents(std::vector<std::vector<float>> contents);

		std::vector<std::vector<float>>& operator[](int i);
		Matrix& operator*(Matrix* m);
		Matrix& operator*=(Matrix* m);
		Matrix& operator/(Matrix* m);
		Matrix& operator/=(Matrix* m);

	protected:
		std::vector < std::vector<float>> content;
	};
}