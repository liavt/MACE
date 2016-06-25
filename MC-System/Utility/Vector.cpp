#include <MC-System/Utility/Vector.h>

template<class T>
std::vector<T> mc::Vector<T>::getContents() const
{
	return this->content;
}

template<class T>
void mc::Vector<T>::setContents(std::vector<T> contents)
{
	this->getContents = contents;
}

template<class T>
std::vector<T>& mc::Vector<T>::operator[](int i)
{
	return content[i];
}

template<class T>
mc::Vector<T>::Vector()
{
}

template<class T>
mc::Vector<T>::Vector(std::vector<T> contents)
{
	this->setContents(contents);
}
