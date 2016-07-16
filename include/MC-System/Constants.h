#pragma once
#define __MACE true //this will be true if MACE is included

#define _MACE_TO_STRING(s) #s

#ifndef __cplusplus
	#error A C++ compiler is required!
#endif 

#include <cstdint>


namespace mc {
	/**
	Primitive type guarenteed to be equal to 8 bits (1 byte)
	*/
	using Byte = uint8_t;

	/**
	Represents the size of an object
	*/
	using Size = unsigned int;

}