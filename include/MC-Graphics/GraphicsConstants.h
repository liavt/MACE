#pragma once

namespace mc {
	namespace gfx {

		//values defining which bit in a byte every propety is, or how much to bit shift it
		/**
		Bit location representing whether an `Entity` is dead.
		<p>
		If `true,` any {@link Container} holding it will remove it and call `kill()`
		@see Entity#getProperty(unsigned int)
		*/
		const Byte ENTITY_PROPERTY_DEAD = 0;
		const Byte ENTITY_PROPERTY_UPDATE_ENABLED = 1;
		const Byte ENTITY_PROPERTY_DIRTY = 2;
		const Byte ENTITY_PROPERTY_INIT = 3;
		const Byte ENTITY_PROPERTY_PASS_DOWN = 4;////

		const Byte POSITION_PROPERTY_STRETCH_X = 0;////
		const Byte POSITION_PROPERTY_STRECTH_Y = 1;////
		const Byte POSITION_PROPERTY_IGNORE_PARENT = 2;////
		const Byte POSITION_PROPERTY_INHERIT_STRETCH_X = 3;////
		const Byte POSITION_PROPERTY_INHERIT_STRETCH_Y = 4;////

#ifdef _MACE_ENTITY_CUSTOM_DEFAULT_PROPERTIES
		const Byte ENTITY_DEFAULT_PROPERTIES = _MACE_ENTITY_CUSTOM_DEFAULT_PROPERTIES;
#else
		const Byte ENTITY_DEFAULT_PROPERTIES = 0b00110110;
#endif

#ifdef _MACE_POSITION_CUSTOM_DEFAULT_PROPERTIES
		const Byte POSITION_DEFAULT_PROPERTIES = _MACE_ENTITY_CUSTOM_DEFAULT_PROPERTIES;
#else
		const Byte POSITION_DEFAULT_PROPERTIES = 0b00000011;
#endif
	}
}