#pragma once
#define _MACE true //this will be true if MACE is included

#define _MACE_TO_STRING(s) #s

#ifndef __cplusplus
	#error A C++ compiler is required!
#endif 

#include <cstdint>


namespace mc {
	using Byte = uint8_t;

	//values defining which bit in a byte every propety is, or how much to bit shift it
	/**
	*Marks an entity for death, where any {@link Container} holding it will remove it
	*/
	const Byte ENTITY_PROPERTY_DEAD = 0;
	const Byte ENTITY_PROPERTY_ENABLED = 1;
	const Byte ENTITY_PROPERTY_DIRTY = 2;
	const Byte ENTITY_PROPERTY_INIT = 3;
	const Byte ENTITY_PROPERTY_INHERIT_ALL = 4;////
	const Byte ENTITY_PROPERTY_PASS_DOWN = 5;////
	const Byte ENTITY_PROPERTY_DELETE_SELF = 6;//when suicide is an answer

	const Byte POSITION_PROPERTY_STRETCH_X=0;////
	const Byte POSITION_PROPERTY_STRECTH_Y = 1;////
	const Byte POSITION_PROPERTY_IGNORE_PARENT = 2;////
	const Byte POSITION_PROPERTY_INHERIT_STRETCH_X = 3;////
	const Byte POSITION_PROPERTY_INHERIT_STRETCH_Y = 4;////


	#ifdef _MACE_ENTITY_CUSTOM_DEFAULT_PROPERTIES
		const Byte ENTITY_DEFAULT_PROPERTIES = _MACE_ENTITY_CUSTOM_DEFAULT_PROPERTIES;
	#else
		const Byte ENTITY_DEFAULT_PROPERTIES = 0b00100110;
	#endif

	#ifdef _MACE_POSITION_CUSTOM_DEFAULT_PROPERTIES
		const Byte POSITION_DEFAULT_PROPERTIES = _MACE_ENTITY_CUSTOM_DEFAULT_PROPERTIES;
	#else
		const Byte POSITION_DEFAULT_PROPERTIES = 0b00000011;
	#endif
}