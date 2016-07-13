#pragma once
#include <MC-System/Constants.h>
#include <iostream>
#include <MC-System/Utility/Math.h>

namespace mc {
	/**
	Wrapper class for a primitive type that allows for easy bit manipulation. Can set individual bits. Additionally, it overrides almost every operator
	to operate on it's wrapped value, to allow for you to manually manipulate it.
	<p>
	To construct it, you can simply call {@code BitField<Byte> field = 0b0000000.} There are constructors for those who want to use them.

	*/
	template<typename T>
	struct BitField {
		/**
		The value this {@code BitField} will be acting upon
		*/
		T value;

		/**
		Default constructor.
		<p>
		Equal to calling {@code BitField(0)}
		*/
		BitField() {
			this->value = 0;
		}

		/**
		Constructs a {@code BitField} with the specified value.
		<p>
		Equal to calling {@code BitField<T> = value}
		@param value Inital value
		*/
		BitField(T value) {
			this->value = value;
		}

		/**
		Cloning constructor
		@param clone Another {@code BitField} to clone the values of
		*/
		BitField(BitField& clone) {
			BitField(clone.value);
		}

		/**
		Default destructor.
		*/
		~BitField() {
			//nothing to delete, just here to complete the constructors
		}

		/**
		Retrieve the value inside of this {@code BitField}
		@return The value represented by this {@code BitField}
		*/
		T get() {
			return value;
		}

		/**
		Change the internal value.
		@param newValue New value for this {@code BitField} to operate on
		*/
		void set(T newValue) {
			value = newValue;
		}

		/**
		Make the bit at a certain position toggled or untoggled
		@param position 0-indexed integer reprsenting which bit to set
		@param state {@code true} to make the specified bit 1, and {@code false} to make it 0
		@returns {@code this} for chaining
		*/
		BitField& setBit(unsigned int position, bool state) {
			if (state) {
				return toggleBit(position);
			}else {
				return untoggleBit(position);
			}
		}

		/**
		Turn bit at {@code position} to be {@code true} or {@code 1}
		@param position 0-indexed integer representing which bit to toggle
		@returns {@code this} for chaining
		*/
		BitField& toggleBit(unsigned int position) {
			value |= (1 << position);
			return *this;
		}

		/**
		Turn bit at {@code position} to be {@code false} or {@code 0}
		@param position 0-indexed integer representing which bit to untoggle
		@returns {@code this} for chaining
		*/
		BitField& untoggleBit(unsigned int position) {
			value &= ~(1 << position);
			return *this;
		}

		/**
		Retrieve the value of a specified bit
		@param position which bit to check
		@returns {@code true} if the bit is 1, {@code false} otherwise
		*/
		bool getBit(unsigned int position) const{
			return (((value >> position) & 1) == 1);
		}
		
		/**
		Inverts a certain bit
		@param position Which bit to "flip," or invert
		@returns {@code this} for chaining
		*/
		BitField& flipBit(unsigned int position) {
			value ^= 1 << position;
			return *this;
		}

		/**
		Inverses every bit, making every 0 a 1 and every 1 a 0.
		<p>
		Equivelant to calling the ~ operator.
		@returns {@code this} for chainign
		*/
		BitField& inverse() {
			value = ~value;
			return *this;
		}

		/**
		Gets how many bits this {@code BitField} is holding
		@return Number of bits
		*/
		Size size() {
			return sizeof(value) * 8;
		}

		//i heard you like operators so i added some operators to your operators
		//dont worry i know how to operate

		//for std::cout
		/**
		Operator for {@link std::cout} to correctly print this class
		*/
		std::ostream &operator<<(std::ostream &os) {
			for (unsigned int i = size() - 1; i >= 0; i--) os << this[i];
			return os;
		}

		/**
		Operator which acts upon the internal value
		@see get()
		*/
		BitField operator|(const T value) {
			return BitField<T>(this->value | value );
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator|=(const T value) {
			this->value |= value;
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		BitField operator&(const T value) {
			return BitField<T>(this->value & value );
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator&=(const T value) {
			this->value &= value;
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		BitField operator^(const T value) {
			return BitField<T>( this->value ^ value);
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator^=(const T value) {
			this->value ^= value;
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		BitField& operator=(const T value) {
			this->value = value;
			return *this;
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		BitField operator|(BitField& other) {
			return BitField<T>(this->value | other.value );
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator|=(BitField& other) {
			this->value |= other.value;
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		BitField operator&(BitField& other) {
			return BitField<T>(this->value & other.value );
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator&=(BitField& other) {
			this->value &= other.value;
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		BitField operator^(BitField& other) {
			return BitField<T>(this->value ^ other.value );
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator^=(BitField& other) {
			this->value ^= other.value;
		}
		/**
		Operator which acts upon the internal value.
		<p>
		Same as calling {@link inverse()}
		@see get()
		*/
		BitField operator~() {
			return BitField<T>((const T)~value );
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		BitField operator>>(unsigned int places) {
			return BitField<T>(value >> places );
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		BitField operator<<(unsigned int places) {
			return BitField<T>(value << places);
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator>>=(unsigned int places) {
			value >>= places;
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator<<=(unsigned int places) {
			value <<= places;
		}
		/**
		Compares this {@code BitField} to another value. Will return {@code true} if the bits represented are both equal.
		*/
		bool operator==(const T other) {
			return value == other;
		}
		/**
		Inverse for {@link operator==}
		*/
		bool operator!=(const T other) {
			return !(value == other);
		}
		/**
		Compares this {@code BitField} to another. Will return {@code true} if the bits represented are both equal.
		*/
		bool operator==(const BitField& other) {
			return value == other.value;
		}
		/**
		Inverse for {@link operator==}
		*/
		bool operator!=(const BitField& other) {
			return (value != other.value);
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		BitField operator+(const T other) const{
			return BitField<T>(value + other );
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		BitField operator+(const BitField& other) const {
			return BitField<T>(value + other.value );
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator+=(const BitField& other) {
			value += other.value;
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator+=(const T other) {
			value += other;
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		BitField operator-(const T other) const{
			return BitField<T>(value - other );
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		BitField operator-(const BitField& other) const {
			return BitField<T>(value - other.value );
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator-=(const BitField& other) {
			value -= other.value;
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator-=(const T other) {
			value -= other;
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		BitField operator*(const T other) {
			return BitField<T>(value * other );
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		BitField operator*(const BitField& other) const {
			return BitField<T>(value * other.value );
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator*=(const BitField& other) {
			value *= other.value;
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator*=(const T other) {
			value *= other;
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		BitField operator/(const T other) const {
			return BitField<T>(value / other );
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		BitField operator/(const BitField& other) const {
			return BitField<T>(value / other.value );
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator/=(const BitField& other) {
			value /= other.value;
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator/=(const T other) {
			value /= other;
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		T operator%(const T other) {
			return value % other;
		}
		/**
		Gets a bit at a certain position
		@param position Which bit to retrieve. Zero-indexed
		@return {@code true} if the bit is 1, {@code false} otherwise.
		@see getBit(unsigned int)
		*/
		bool operator[](unsigned int position) {
			return getBit(position);
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator++() {
			value++;
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator++(int dummy) {
			++value;
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator--() {
			value--;
		}
		/**
		Operator which acts upon the internal value
		@see get()
		*/
		void operator--(int dummy) {
			--value;
		}
	};

	/**
	{@link BitField} which wraps around a {@link Byte byte,} meaning that it will have 8 bits.
	*/
	using ByteField = BitField<Byte>;
}