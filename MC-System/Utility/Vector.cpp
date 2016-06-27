#include <MC-System/Utility/Vector.h>


template<typename T, int N>
std::array<T,N>* mc::Vector<T,N>::getContents() const
{
	return this->content;
}

template<typename T, int N>
void mc::Vector<T,N>::setContents(std::array<T,N> contents)
{
	this->content = contents;
}

template<typename T, int N>
int mc::Vector<T,N>::size()
{
	return N;
}

template<typename T, int N>
T * mc::Vector<T,N>::begin()
{
	return content.begin();
}

template<typename T, int N>
T * mc::Vector<T,N>::end()
{
	return content.end();
}

template<typename T, int N>
T& mc::Vector<T, N>::operator[](int i)
{
	return content[i];
}

template<typename T, int N>
template<typename TOther, int NOther>
mc::Vector<T,N> mc::Vector<T, N>::operator*(const Vector<TOther,NOther> & m) const
{
	int outSize = NOther >= N ? N : NOther;

	std::vector<T, N> out = std::vector<T, N>(this);
	for (int i = 0; i < outSize; i++) {
		out[i] = (this[i] * m[i]);
	}
	return mc::Vector<T, N>(out);
}



template<typename T, int N>
mc::Vector<T,N> mc::Vector<T,N>::operator*=(const mc::Vector<T,N> & m)
{
	return this*m;
}

template<typename T, int N>
template<typename TOther, int NOther>
mc::Vector<T, N> mc::Vector<T, N>::operator/(const Vector<TOther, NOther> & m) const
{
	int outSize = NOther >= N ? N : NOther;

	std::vector<T, N> out = std::vector<T, N>(this);
	for (int i = 0; i < outSize; i++) {
		out[i] = (this[i] / m[i]);
	}
	return mc::Vector<T, N>(out);
}


template<typename T, int N>
mc::Vector<T,N> mc::Vector<T,N>::operator/=(const mc::Vector<T,N> & m)
{
	return this/m;
}

template<typename T, int N>
template<typename TOther, int NOther>
mc::Vector<T, N> mc::Vector<T, N>::operator+(const Vector<TOther, NOther> & m) const
{
	int outSize = NOther >= N ? N : NOther;

	std::vector<T, N> out = std::vector<T, N>(this);
	for (int i = 0; i < outSize; i++) {
		out[i] = (this[i] + m[i]);
	}
	return mc::Vector<T, N>(out);
}


template<typename T, int N>
mc::Vector<T,N> mc::Vector<T,N>::operator+=(const mc::Vector<T,N> & m)
{
	return this+m;
}

template<typename T, int N>
template<typename TOther, int NOther>
mc::Vector<T, N> mc::Vector<T, N>::operator-(const Vector<TOther, NOther> & m) const
{
	int outSize = NOther >= N ? N : NOther;

	std::vector<T, N> out = std::vector<T, N>(this);
	for (int i = 0; i < outSize; i++) {
		out[i] = (this[i] - m[i]);
	}
	return mc::Vector<T, N>(out);
}


template<typename T, int N>
mc::Vector<T,N> mc::Vector<T,N>::operator-=(const mc::Vector<T,N> & m)
{
	return this-m;
}

template<typename T, int N>
bool mc::Vector<T,N>::operator==(const mc::Vector<T,N> & other)
{
	const int* size = this->size();
	if (size != other.size())return false;
	for (int i = 0; i < size(); i++) {
		if (this[i] != other[i]) {
			return false;
		}
	}
	return true;
}

template<typename T, int N>
bool mc::Vector<T,N>::operator!=(const mc::Vector<T,N> & other)
{
	return !(this==other);
}

template<typename T, int N>
mc::Vector<T,N>::Vector()
{
	this->setContents(std::array<T,N>());
}

template<typename T, int N>
mc::Vector<T, N>::Vector(std::array<T, N>& contents)
{
	this->setContents(contents);
}

template<typename T, int N>
mc::Vector<T, N>::Vector(const Vector & obj)
{
	this->setContents(obj->getContents());
}
