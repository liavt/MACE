#pragma once
#define _MACE true //this will be true if MACE is included

#define _MACE_toString(c) #c

//used for bit masking
#define _MACE_TRUE 0b10000000
#define _MACE_FALSE 0b00000000

#ifndef __cplusplus
	#error A C++ compiler is required!
#endif 

//for entities:
#ifndef _MACE_ENTITY_DEFAULT_CONTAINER_SIZE
	#define _MACE_ENTITY_DEFAULT_CONTAINER_SIZE 0
#endif
#ifndef _MACE_ENTITY_DEFAULT_PROPERTIES
	#define _MACE_ENTITY_DEFAULT_PROPERTIES 0b01110101
#endif

//values defining which bit in a byte every propety is, or how much to bit shift it
#define _MACE_ENTITY_DEAD 0
#define _MACE_ENTITY_ENABLED 1
#define _MACE_ENTITY_DIRTY 2
#define _MACE_PASS_DOWN_PROPERTIES 3
#define _MACE_ENTITY_STRECH_X 4
#define _MACE_ENTITY_INHERIT_STRETCH_X 5
#define _MACE_ENTITY_STRETCH_Y 6
#define _MACE_ENTITY_IHERIT_STRETCH_Y 7

namespace mc {
	using Bit8 = unsigned char;

	using Byte = Bit8;


}