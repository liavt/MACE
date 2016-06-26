#include <MC-System/Utility/Vector.h>

template<class T>
std::vector<T>* mc::Vector<T>::getContents() const
{
	return this->content;
}

template<class T>
void mc::Vector<T>::setContents(std::vector<T> contents)
{
	this->getContents = contents;
}

template<class T>
int * mc::Vector<T>::size()
{
	return sizeof(this)/sizeof(T);
}

template<class T>
T * mc::Vector<T>::begin()
{
	return content.begin();
}

template<class T>
T * mc::Vector<T>::end()
{
	return content.end();
}

template<class T>
T& mc::Vector<T>::operator[](int i)
{
	return content[i];
}

template<class T>
mc::Vector<T> mc::Vector<T>::operator*(const mc::Vector<T> & m) const
{
	int outSize;
	if (m.size() >= this->size()) {
		outSize = this->size()
	}
	else if (m.size() < this->size()) {
		outSize = m.size()
	}

	std::vector<T> out = std::vector<T>(outSize);
	for (int i = 0; i < outSize; i++) {
		out.push_back(this[i] * m[i]);
	}
	return mc::Vector<T>(out);
}

template<class T>
mc::Vector<T> mc::Vector<T>::operator*=(const mc::Vector<T> & m)
{
	return this*m;
}

template<class T>
mc::Vector<T> mc::Vector<T>::operator/(const mc::Vector<T> & m)  const
{
	int outSize;
	if (m.size() >= this->size()) {
		outSize = this->size()
	}
	else if (m.size() < this->size()) {
		outSize = m.size()
	}

	std::vector<T> out = std::vector<T>(outSize);
	for (int i = 0; i < outSize; i++) {
		out.push_back(this[i] / m[i]);
	}
	return mc::Vector<T>(out);
}

template<class T>
mc::Vector<T> mc::Vector<T>::operator/=(const mc::Vector<T> & m)
{
	return this/m;
}

template<class T>
mc::Vector<T> mc::Vector<T>::operator+(const mc::Vector<T> & m) const
{
	int outSize;
	if (m.size() >= this->size()) {
		outSize = this->size()
	}else if (m.size() < this->size()) {
		outSize = m.size()
	}

	std::vector<T> out = std::vector<T>(outSize);
	for (int i = 0; i < outSize; i++) {
		out.push_back(this[i]+m[i]);
	}
	return mc::Vector<T>(out);
}

template<class T>
mc::Vector<T> mc::Vector<T>::operator+=(const mc::Vector<T> & m)
{
	return this+m;
}

template<class T>
mc::Vector<T> mc::Vector<T>::operator-(const mc::Vector<T> & m) const
{
	int outSize;
	if (m.size() >= this->size()) {
		outSize = this->size()
	}
	else if (m.size() < this->size()) {
		outSize = m.size()
	}

	std::vector<T> out = std::vector<T>(outSize);
	for (int i = 0; i < outSize; i++) {
		out.push_back(this[i] - m[i]);
	}
	return mc::Vector<T>(out);
}

template<class T>
mc::Vector<T> mc::Vector<T>::operator-=(const mc::Vector<T> & m)
{
	return this-m;
}

template<class T>
bool mc::Vector<T>::operator==(const mc::Vector<T> & other)
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

template<class T>
bool mc::Vector<T>::operator!=(const mc::Vector<T> & other)
{
	return !(this==other);
}

template<class T>
mc::Vector<T>::Vector()
{
}

template<class T>
mc::Vector<T>::Vector(std::vector<T> *contents)
{
	this->setContents(contents);
}
