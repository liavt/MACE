/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Catch.h>
#include <MC-System/Utility/BitField.h>

namespace mc{
	TEST_CASE("Testing BitField as a number","[system][utility][bitfield]") {
		ByteField b = 10;
		REQUIRE(b==10);
		b += (5);
		REQUIRE(b==15);
		b -= 10;
		REQUIRE(b==5);
		b *= 5;
		REQUIRE(b==25);
		b /= 5;
		REQUIRE(b==5);
		REQUIRE(b+5==10);
		REQUIRE(b-3==2);
		REQUIRE(b*25==125);
		REQUIRE(b/5==1);
		REQUIRE(b%3==2);

		b++;
		REQUIRE((b==6));
		b--;
		REQUIRE(b==5);
	}

	TEST_CASE("Testing size()","[system][utility][bitfield]"){
		BitField<Byte> b = 0b0000000;
		REQUIRE(b.size()==8);
		
		BitField<uint16_t> b2 = 0b0;
		REQUIRE(b2.size()==16);
	}

	TEST_CASE("Testing BitField bitwise operators", "[system][utility][bitfield]") {
		ByteField b = { 0b00000000 };

		b |= (0b01010101);

		REQUIRE(b == 0b01010101);
		REQUIRE(b!= 0b11111111);

		b &= 0b00001111;

		REQUIRE(b == 0b00000101);

		b ^= 0b11111111;

		REQUIRE(b == 0b11111010);

		b = BitField<Byte>(0b01001000);

		b <<= 1;

		REQUIRE(b == 0b10010000);

		b = 0b00000000;

		REQUIRE(b == 0b00000000);

		b = b | (0b01010101);

		REQUIRE(b == 0b01010101);

		b = b & 0b00001111;

		REQUIRE(b == 0b00000101);

		b = b ^ 0b11111111;

		REQUIRE(b == 0b11111010);

		b = BitField<Byte>(0b01001000);

		b = b<< 1;

		REQUIRE(b == 0b10010000);
	}

	TEST_CASE("Testing Bitfield creation", "[system][utility][bitfield]") {
		ByteField b = ByteField(static_cast<Byte>(3));
		REQUIRE(b== static_cast<Byte>(3));
	 	b = static_cast<Byte>(32);
		REQUIRE(b== static_cast<Byte>(32));
		b = BitField<Byte>((0b10101010));
		REQUIRE(b== (0b10101010));
		b = ByteField(BitField<Byte>(static_cast<Byte>(56)));//copy constructor.
		REQUIRE(b== static_cast<Byte>(56));
		b = ByteField();
		REQUIRE(b== static_cast<Byte>(0));
	}

	TEST_CASE("Testing BitField functions", "[system][utility][bitfield]") {
		BitField<Byte> b = 0b00000000;

		REQUIRE(b==0b0000000);

		for (unsigned int i = 0; i < 8; i++) {
			REQUIRE(!b[0]);
			REQUIRE(!b.getBit(i));
		}

		b.inverse();

		for (unsigned int i = 0; i < 8; i++) {
			REQUIRE(b[0]);
			REQUIRE(b.getBit(i));
		}

		b.setBit(3,true);
		b.setBit(5, false);

		REQUIRE(b.getBit(3));
		REQUIRE(!b.getBit(5));

		b.flipBit(3);

		REQUIRE(!b.getBit(3));

		b.toggleBit(5);

		REQUIRE(b.getBit(5));
		
		b.untoggleBit(1);

		REQUIRE(!b.getBit(1));

		REQUIRE(sizeof(b)==1);
	}
}