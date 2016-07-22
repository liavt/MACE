#pragma once
#include <MC-System/Constants.h>
#include <iostream>
#include <MC-System/Utility/Math.h>

namespace mc {
	/**
	Wrapper class for a primitive type that allows for easy bit manipulation. Can set individual bits. Additionally, it overrides almost every operator
	to operate on it's wrapped value, to allow for you to manually manipulate it.
	<p>
	Examples:
	{@code
		BitField<int> field = 15;//Creation
		
		field.setBit(i,state)//Set bit in position i to state

		field.getBit(i) //Get whether bit in position i is true or false
	}
	@tparam T What primitive the`BitField` should use to store the bits
	*/
	template<typename T>
	struct BitField {
		/**
		The value this `BitField` will be acting upon
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
		Constructs a `BitField` with the specified value.
		<p>
		Equal to calling {@code BitField<T> = value}
		@param value Inital value
		*/
		BitField(const T value) {
			this->value = value;
		}

		/**
		Cloning constructor
		@param clone Another `BitField` to clone the values of
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
		Retrieve the `const` value inside of this `BitField`
		@return The value represented by this `BitField`
		@see get()
		*/
		const T get() const {
			return value;
		}
		/**
		Retrieve the value inside of this `BitField`
		@return The value represented by this `BitField`
		@see get() const
		*/
		T get() {
			return value;
		}

		/**
		Change the internal value.
		@param newValue New value for this `BitField` to operate on
		*/
		void set(T newValue) {
			value = newValue;
		}

		/**
		Make the bit at a certain position toggled or untoggled
		@param position 0-indexed integer reprsenting which bit to set
		@param state `true` to make the specified bit 1, and `false` to make it 0
		@return `this` for chaining
		*/
		BitField& setBit(Index position, bool state) {
			if (state) {
				return toggleBit(position);
			}else {
				return untoggleBit(position);
			}
		}

		/**
		Turn bit at `position` to be `true` or `1`
		@param position 0-indexed integer representing which bit to toggle
		@return `this` for chaining
		*/
		BitField& toggleBit(Index position) {
			value |= (1 << position);
			return *this;
		}

		/**
		Turn bit at `position` to be `false` or `0`
		@param position 0-indexed integer representing which bit to untoggle
		@return `this` for chaining
		*/
		BitField& untoggleBit(Index position) {
			value &= ~(1 << position);
			return *this;
		}

		/**
		Retrieve the value of a specified bit
		@param position which bit to check
		@return `true` if the bit is 1, `false` otherwise
		*/
		bool getBit(Index position) const{
			return (((value >> position) & 1) == 1);
		}
		
		/**
		Inverts a certain bit
		@param position Which bit to "flip," or invert
		@return `this` for chaining
		*/
		BitField& flipBit(Index position) {
			value ^= 1 << position;
			return *this;
		}

		/**
		Inverses every bit, making every 0 a 1 and every 1 a 0.
		<p>
		Equivelant to calling the ~ operator.
		@return `this` for chainign
		*/
		BitField& inverse() {
			value = ~value;
			return *this;
		}

		/**
		Gets how many bits this `BitField` is holding
		@return Number of bits
		*/
		Size size() const {
			return sizeof(value) * 8;
		}

		//i heard you like operators so i added some operators to your operators
		//dont worry i know how to operate

		//for std::cout
		/**
		Operator for `std::cout` to correctly print this class
		*/
		std::ostream &operator<<(BitField<T> b) {
			for (Index i = size() - 1; i >= 0; i--) os << b[i];
			return os;
		}

		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator|(const T value) {
			return BitField<T>(this->value | value );
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator|=(const T value) {
			this->value |= value;
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator&(const T value) {
			return BitField<T>(this->value & value );
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator&=(const T value) {
			this->value &= value;
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator^(const T value) {
			return BitField<T>( this->value ^ value);
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator^=(const T value) {
			this->value ^= value;
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator|(BitField& other) {
			return BitField<T>(this->value | other.value );
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator|=(BitField& other) {
			this->value |= other.value;
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator&(BitField& other) {
			return BitField<T>(this->value & other.value );
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator&=(BitField& other) {
			this->value &= other.value;
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator^(BitField& other) {
			return BitField<T>(this->value ^ other.value );
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator^=(BitField& other) {
			this->value ^= other.value;
		}
		/**
		Operator which acts upon the internal value.
		<p>
		Same as calling {@link inverse()}
		@see get() @see value
		*/
		BitField operator~() {
			return BitField<T>((const T)~value );
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator>>(Index places) {
			return BitField<T>(value >> places );
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator<<(Index places) {
			return BitField<T>(value << places);
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator>>=(Index places) {
			value >>= places;
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator<<=(Index places) {
			value <<= places;
		}
		/**
		Compares this `BitField` to another value. Will return `true` if the bits represented are both equal.
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
		Compares this `BitField` to another. Will return `true` if the bits represented are both equal.
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
		@see get() @see value
		*/
		BitField operator+(const T other) const{
			return BitField<T>(value + other );
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator+(const BitField& other) const {
			return BitField<T>(value + other.value );
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator+=(const BitField& other) {
			value += other.value;
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator+=(const T other) {
			value += other;
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator-(const T other) const{
			return BitField<T>(value - other );
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator-(const BitField& other) const {
			return BitField<T>(value - other.value );
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator-=(const BitField& other) {
			value -= other.value;
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator-=(const T other) {
			value -= other;
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator*(const T other) {
			return BitField<T>(value * other );
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator*(const BitField& other) const {
			return BitField<T>(value * other.value );
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator*=(const BitField& other) {
			value *= other.value;
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator*=(const T other) {
			value *= other;
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator/(const T other) const {
			return BitField<T>(value / other );
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator/(const BitField& other) const {
			return BitField<T>(value / other.value );
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator/=(const BitField& other) {
			value /= other.value;
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator/=(const T other) {
			value /= other;
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		T operator%(const T other) {
			return value % other;
		}
		/**
		Gets a bit at a certain position
		@param position Which bit to retrieve. Zero-indexed
		@return `true` if the bit is 1, `false` otherwise.
		@see getBit(Index)
		*/
		bool operator[](Index position) {
			return getBit(position);
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator++() {
			value++;
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator++(int dummy) {
			++value;
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator--() {
			value--;
		}
		/**
		Operator which acts upon the internal value
		@see get() @see value
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