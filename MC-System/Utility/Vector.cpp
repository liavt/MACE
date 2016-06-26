#include <MC-System/Utility/Vector.h>


template<class T, int N>
std::array<T,N>* mc::Vector<T,N>::getContents() const
{
	return this->content;
}

template<class T, int N>
void mc::Vector<T,N>::setContents(std::array<T,N> contents)
{
	this->getContents = contents;
}

template<class T, int N>
int * mc::Vector<T,N>::size()
{
	return sizeof(this)/sizeof(T);
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
	int outSize;
	if (m.size() >= this->size()) {
		outSize = this->size()
	}
	else if (m.size() < this->size()) {
		outSize = m.size()
	}

	std::array<T,N> out = std::array<T,N>();
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
	int outSize;
	if (m.size() >= this->size()) {
		outSize = this->size()
	}
	else if (m.size() < this->size()) {
		outSize = m.size()
	}


	return mc::Vector<T,N>(out);
}

template<class T, int N>
mc::Vector<T,N> mc::Vector<T,N>::operator/=(const mc::Vector<T,N> & m)
{
	return this/m;
}

template<class T, int N>
mc::Vector<T,N> mc::Vector<T,N>::operator+(const mc::Vector<T,N> & m) const
{
	int outSize;
	if (m.size() >= this->size()) {
		outSize = this->size()
	}else if (m.size() < this->size()) {
		outSize = m.size()
	}


	return mc::Vector<T,N>(out);
}

template<class T, int N>
mc::Vector<T,N> mc::Vector<T,N>::operator+=(const mc::Vector<T,N> & m)
{
	return this+m;
}

template<class T, int N>
mc::Vector<T,N> mc::Vector<T,N>::operator-(const mc::Vector<T,N> & m) const
{
	int outSize;
	if (m.size() >= this->size()) {
		outSize = this->size()
	}
	else if (m.size() < this->size()) {
		outSize = m.size()
	}


	return mc::Vector<T,N>(out);
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
}

template<class T, int N>
mc::Vector<T, N>::Vector(std::array<T, N>& contents)
{
	this->setContents(contents);
}
