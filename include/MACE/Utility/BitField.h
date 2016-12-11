/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_UTILITY_BITFIELD_H
#define MACE_UTILITY_BITFIELD_H

#include <MACE/System/Constants.h>
#include <ostream>

namespace mc {
	/**
	Wrapper class for a primitive type that allows for easy bit manipulation. Can set individual bits. Additionally, it overrides almost every operator
	to operate on it's value, to allow for you to manually manipulate it.
	<p>
	It is guarenteed to contain 8 bits. On systems where an unsigned char is 8 bits, sizeof(BitField) will be 1.
	<p>
	Examples:
	{@code
	BitField field = 15;//Creation

	field.setBit(i,state)//Set bit in position i to state

	field.getBit(i) //Get whether bit in position i is true or false
	}
	*/
	struct BitField {
		/**
		The value this `BitField` will be acting upon
		*/

		Byte value;

		/**
		Default constructor.
		<p>
		Equal to calling {@code BitField(0)}
		*/
		BitField() noexcept;

		/**
		Constructs a `BitField` with the specified value.
		<p>
		Equal to calling {@code BitField = value}
		@param val Inital value
		*/
		BitField(const Byte val) noexcept;


		/**
		Default destructor.
		*/
		~BitField() noexcept;

		/**
		Make the bit at a certain position toggled or untoggled
		@param position 0-indexed integer reprsenting which bit to set
		@param state `true` to make the specified bit 1, and `false` to make it 0
		@return `this` for chaining
		*/
		BitField& setBit(Byte position, bool state);

		/**
		Turn bit at `position` to be `true` or `1`
		@param position 0-indexed integer representing which bit to toggle
		@return `this` for chaining
		*/
		BitField& toggleBit(Byte position);

		/**
		Turn bit at `position` to be `false` or `0`
		@param position 0-indexed integer representing which bit to untoggle
		@return `this` for chaining
		*/
		BitField& untoggleBit(Byte position);

		/**
		Retrieve the value of a specified bit
		@param position which bit to check
		@return `true` if the bit is 1, `false` otherwise
		*/
		bool getBit(Byte position) const;

		/**
		Inverts a certain bit
		@param position Which bit to "flip," or invert
		@return `this` for chaining
		*/
		BitField& flipBit(Byte position);

		/**
		Inverses every bit, making every 0 a 1 and every 1 a 0.
		<p>
		Equivelant to calling the ~ operator.
		@return `this` for chainign
		*/
		BitField& inverse();

		//i heard you like operators so i added some operators to your operators
		//dont worry i know how to operate

		//for std::cout
		/**
		Operator for `std::cout` to correctly print this class
		*/
		friend std::ostream& operator<<(std::ostream &os, const BitField& b) {
			//has to have 8 bits, so we countdown from 8
			for( Byte i = 0; i < 8; i++ )os << b.getBit((7) - i);//0 indexed so that explains the magic number of 7
			return os;
		}

		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator|(const Byte value) const;
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator|=(const Byte value);
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator&(const Byte value) const;
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator&=(const Byte value);
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator^(const Byte value) const;
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator^=(const Byte value);
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator|(const BitField& other) const;
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator|=(const BitField& other);
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator&(const BitField& other) const;
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator&=(const BitField& other);
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator^(const BitField& other) const;
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator^=(const BitField& other);
		/**
		Operator which acts upon the internal value.
		<p>
		Same as calling {@link inverse()}
		@see get() @see value
		*/
		BitField operator~();
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator>>(const Index places) const;
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator<<(const Index places) const;
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator>>=(const Index places);
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator<<=(const Index places);
		/**
		Compares this `BitField` to another value. Will return `true` if the bits represented are both equal.
		*/
		bool operator==(const Byte other) const noexcept;
		/**
		Inverse for {@link operator==}
		*/
		bool operator!=(const Byte other) const noexcept;
		/**
		Compares this `BitField` to another. Will return `true` if the bits represented are both equal.
		*/
		bool operator==(const BitField& other) const noexcept;
		/**
		Inverse for {@link operator==}
		*/
		bool operator!=(const BitField& other) const noexcept;
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator+(const Byte other) const;
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator+(const BitField& other) const;
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator+=(const BitField& other);
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator+=(const Byte other);
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator-(const Byte other) const;
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator-(const BitField& other) const;
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator-=(const BitField& other);
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator-=(const Byte other);
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator*(const Byte other);
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator*(const BitField& other) const;
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator*=(const BitField& other);
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator*=(const Byte other);
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator/(const Byte other) const;
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		BitField operator/(const BitField& other) const;
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator/=(const BitField& other);
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator/=(const Byte other);
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		Byte operator%(const Byte other) const;
		/**
		Gets a bit at a certain position
		@param position Which bit to retrieve. Zero-indexed
		@return `true` if the bit is 1, `false` otherwise.
		@see getBit(Byte)
		*/
		bool operator[](const Byte position);
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator++();
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator++(int dummy);
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator--();
		/**
		Operator which acts upon the internal value
		@see get() @see value
		*/
		void operator--(int dummy);
	};//BitField
}//mc

#endif