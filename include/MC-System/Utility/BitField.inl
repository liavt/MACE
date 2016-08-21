
template<typename T>
inline BitField<T>::BitField(const T val) {
	this->value = val;
}

template<typename T>
inline BitField<T>::BitField() : value(){
}

template<typename T>
inline BitField<T>::~BitField() {
//nothing to destroy, just here in case later on someone adds a constructor
}
template<typename T>
inline const T BitField<T>::get() const {
	return value;
}

template<typename T>
inline T BitField<T>::get() {
	return value;
}

template<typename T>
inline void BitField<T>::set(const T newValue){
	value = newValue;
}

template<typename T>
inline BitField<T>& BitField<T>::setBit(const Index position, const bool state) {
	if (state) {
		return toggleBit(position);
	}
	else {
		return untoggleBit(position);
	}
}

template<typename T>
inline BitField<T>& BitField<T>::toggleBit(const Index position) {
	value |= (1 << position);
	return *this;
}

template<typename T>
inline BitField<T>& BitField<T>::untoggleBit(const Index position) {
	value &= ~(1 << position);
	return *this;
}

template<typename T>
inline bool BitField<T>::getBit(const Index position) const {
	return (((value >> position) & 1) == 1);
}

template<typename T>
inline BitField<T>& BitField<T>::flipBit(const Index position) {
	value ^= 1 << position;
	return *this;
}

template<typename T>
inline BitField<T>& BitField<T>::inverse() {
	value = ~value;
	return *this;
}

template<typename T>
inline Size BitField<T>::size() const {
	return sizeof(value) * (CHAR_BIT);
}

template<typename T>
inline T BitField<T>::operator%(const T other) {
	return value % other;
}

template<typename T>
inline BitField<T> BitField<T>::operator~() {
	return BitField<T>((const T)~value);
}

template<typename T>
std::ostream & mc::operator<<(std::ostream & os, const BitField<T>& b)
{
	for (Index i = 0; i < b.size(); i++)os << b.getBit((b.size() - 1) - i);
	return os;
}

template<typename T>
inline bool BitField<T>::operator[](const Index position) {
	return getBit(position);
}