#include <MC-Testing/Catch.h>
#include <MC-System/Utility/BitField.h>
#include <iostream>

namespace mc{
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