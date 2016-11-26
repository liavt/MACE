#include <MACE/Utility/BitField.h>

namespace mc {

	//default initilize value, incase it is not integral
	BitField::BitField() : value() {}

	BitField::BitField(const Byte val) : value(val) {}

	//nothing to delete, just here to complete the constructor trifecta
	BitField::~BitField() {}

	BitField& BitField::setBit(Byte position, bool state) {
		if( state ) {
			return toggleBit(position);
		} else {
			return untoggleBit(position);
		}
	}

	BitField& BitField::toggleBit(Byte position) {
		value |= (1 << position);
		return *this;
	}

	BitField& BitField::untoggleBit(Byte position) {
		value &= ~(1 << position);
		return *this;
	}

	bool BitField::getBit(Byte position) const {
		return ((((value >> position) & 0x1))) == 1;
	}

	BitField& BitField::flipBit(Byte position) {
		value ^= 1 << position;
		return *this;
	}

	BitField& BitField::inverse() {
		value = ~value;
		return *this;
	}


	BitField BitField::operator|(const Byte val) {
		return BitField(this->value | val);
	}

	void BitField::operator|=(const Byte val) {
		this->value |= val;
	}

	BitField BitField::operator&(const Byte val) {
		return BitField(this->value & val);
	}

	void BitField::operator&=(const Byte val) {
		this->value &= val;
	}

	BitField BitField::operator^(const Byte val) {
		return BitField(this->value ^ val);
	}

	void BitField::operator^=(const Byte val) {
		this->value ^= val;
	}

	BitField BitField::operator|(BitField& other) {
		return BitField(this->value | other.value);
	}

	void BitField::operator|=(BitField& other) {
		this->value |= other.value;
	}

	BitField BitField::operator&(BitField& other) {
		return BitField(this->value & other.value);
	}

	void BitField::operator&=(BitField& other) {
		this->value &= other.value;
	}

	BitField BitField::operator^(BitField& other) {
		return BitField(this->value ^ other.value);
	}

	void BitField::operator^=(BitField& other) {
		this->value ^= other.value;
	}

	BitField BitField::operator~() {
		return BitField((const Byte) ~value);
	}

	BitField BitField::operator>>(Index places) {
		return BitField(value >> places);
	}

	BitField BitField::operator<<(Index places) {
		return BitField(value << places);
	}

	void BitField::operator>>=(Index places) {
		value >>= places;
	}

	void BitField::operator<<=(Index places) {
		value <<= places;
	}

	bool BitField::operator==(const Byte other) {
		return value == other;
	}

	bool BitField::operator!=(const Byte other) {
		return (value != other);
	}

	bool BitField::operator==(const BitField& other) {
		return value == other.value;
	}

	bool BitField::operator!=(const BitField& other) {
		return (value != other.value);
	}

	BitField BitField::operator+(const Byte other) const {
		return BitField(value + other);
	}

	BitField BitField::operator+(const BitField& other) const {
		return BitField(value + other.value);
	}

	void BitField::operator+=(const BitField& other) {
		value += other.value;
	}

	void BitField::operator+=(const Byte other) {
		value += other;
	}

	BitField BitField::operator-(const Byte other) const {
		return BitField(value - other);
	}

	BitField BitField::operator-(const BitField& other) const {
		return BitField(value - other.value);
	}

	void BitField::operator-=(const BitField& other) {
		value -= other.value;
	}

	void BitField::operator-=(const Byte other) {
		value -= other;
	}

	BitField BitField::operator*(const Byte other) {
		return BitField(value * other);
	}

	BitField BitField::operator*(const BitField& other) const {
		return BitField(value * other.value);
	}

	void BitField::operator*=(const BitField& other) {
		value *= other.value;
	}

	void BitField::operator*=(const Byte other) {
		value *= other;
	}

	BitField BitField::operator/(const Byte other) const {
		return BitField(value / other);
	}

	BitField BitField::operator/(const BitField& other) const {
		return BitField(value / other.value);
	}

	void BitField::operator/=(const BitField& other) {
		value /= other.value;
	}

	void BitField::operator/=(const Byte other) {
		value /= other;
	}

	Byte BitField::operator%(const Byte other) {
		return value % other;
	}

	bool BitField::operator[](Byte position) {
		return getBit(position);
	}

	void BitField::operator++() {
		value++;
	}

	void BitField::operator++(int) {
		++value;
	}

	void BitField::operator--() {
		value--;
	}

	void BitField::operator--(int) {
		--value;
	}
}//mc