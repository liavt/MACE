#include <MC-System/Utility/Vector.h>


template<class T, int N>
std::array<T,N>* mc::Vector<T,N>::getContents() const
{
	return this->content;
}

template<class T, int N>
void mc::Vector<T,N>::setContents(std::array<T,N> contents)
{
	this->content = contents;
}

template<class T, int N>
int mc::Vector<T,N>::size()
{
	return N;
}

template<class T, int N>
T * mc::Vector<T,N>::begin()
{
	return content.begin();
}

template<class T, int N>
T * mc::Vector<T,N>::end()
{
	return content.end();
}

template<class T, int N>
T& mc::Vector<T,N>::operator[](int i)
{
	return content[i];
}

template<class T, int N>
mc::Vector<T,N> mc::Vector<T,N>::operator*(const mc::Vector<T,N> & m) const
{
	int outSize = m.size()>=this->size() ? this->size() : m.size();

	std::Vector<T,N> out = std::Vector<T,N>(this);
	for (int i = 0; i < outSize; i++) {
		out[i]=(this[i] * m[i]);
	}
	return mc::Vector<T,N>(out);
}

template<class T, int N>
mc::Vector<T,N> mc::Vector<T,N>::operator*=(const mc::Vector<T,N> & m)
{
	return this*m;
}

template<class T, int N>
mc::Vector<T,N> mc::Vector<T,N>::operator/(const mc::Vector<T,N> & m)  const
{
	int outSize = m.size() >= this->size() ? this->size() : m.size();

	std::Vector<T, N> out = std::Vector<T, N>(this);
	for (int i = 0; i < outSize; i++) {
		out[i] = (this[i] / m[i]);
	}
	return mc::Vector<T, N>(out);
}

template<class T, int N>
mc::Vector<T,N> mc::Vector<T,N>::operator/=(const mc::Vector<T,N> & m)
{
	return this/m;
}

template<class T, int N>
mc::Vector<T,N> mc::Vector<T,N>::operator+(const mc::Vector<T,N> & m) const
{
	int outSize = m.size() >= this->size() ? this->size() : m.size();

	std::Vector<T, N> out = std::Vector<T, N>(this);
	for (int i = 0; i < outSize; i++) {
		out[i] = (this[i] + m[i]);
	}
	return mc::Vector<T, N>(out);
}

template<class T, int N>
mc::Vector<T,N> mc::Vector<T,N>::operator+=(const mc::Vector<T,N> & m)
{
	return this+m;
}

template<class T, int N>
mc::Vector<T,N> mc::Vector<T,N>::operator-(const mc::Vector<T,N> & m) const
{
	int outSize = m.size() >= this->size() ? this->size() : m.size();

	std::Vector<T, N> out = std::Vector<T, N>(this);
	for (int i = 0; i < outSize; i++) {
		out[i] = (this[i] - m[i]);
	}
	return mc::Vector<T, N>(out);
}

template<class T, int N>
mc::Vector<T,N> mc::Vector<T,N>::operator-=(const mc::Vector<T,N> & m)
{
	return this-m;
}

template<class T, int N>
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

template<class T, int N>
bool mc::Vector<T,N>::operator!=(const mc::Vector<T,N> & other)
{
	return !(this==other);
}

template<class T, int N>
mc::Vector<T,N>::Vector()
{
	this->setContents(std::array<T,N>());
}

template<class T, int N>
mc::Vector<T, N>::Vector(std::array<T, N>& contents)
{
	this->setContents(contents);
}

template<class T, int N>
mc::Vector<T, N>::Vector(const Vector & obj)
{
	this->setContents(obj->getContents());
}

template<class T, int W, int H>
int mc::Matrix<T, W, H>::size()
{
	return W*H;
}

template<class T, int W, int H>
int mc::Matrix<T, W, H>::width()
{
	return W;
}

template<class T, int W, int H>
int mc::Matrix<T, W, H>::height()
{
	return H;
}