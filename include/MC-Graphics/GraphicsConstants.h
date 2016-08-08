/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#include <MC-System/Constants.h>
#include <MC-System/Exceptions.h>

namespace mc {
	namespace gfx {

		struct GLError : public Exception {
			using Exception::Exception;
		};

		struct ShaderError : public Exception {
			using Exception::Exception;
		};

		//values defining which bit in a byte every propety is, or how much to bit shift it
		/**
		Bit location representing whether an `Entity` is dead.
		<p>
		If `true,` any {@link Container} holding it will remove it and call `kill()`
		@see Entity#getProperty(unsigned int)
		*/
		const Byte ENTITY_DEAD = 0;
		/**
		Property defining if an `Entity` can be updated. If this is `true`, `update()` will be called by it's parent.
		@see Entity#getProperty(unsigned int)
		*/
		const Byte ENTITY_UPDATE_ENABLED = 1;
		/**
		Property defining if an `Entity` can be rendered. If this is `true`, `render()` will be called by it's parent.
		@see Entity#getProperty(unsigned int)
		*/
		const Byte ENTITY_RENDER_ENABLED = 2;

		//why are there slashes after everything? its to remember what needs to be worked on!
		/*
		// double slashes means that inheritence works, but its actual function has not been tested
		/// triple slashes means that is has been implemented, but nothing about it has been tested
		//// quadriple slashes means that it has not been implemented
		*/

		/**
		Flag representing whether an Entity's init() function has been called.
		<p>
		If destroy() or update() is called and this is `false`, an `InitializationError` is thrown.
		<p>
		If init() is called and this is `true`, an `InitializationError` is thrown.
		@see Entity#getProperty(unsigned int)
		*/
		const Byte ENTITY_INIT = 4;
		/**
		Flag representing whether the children of an Entity should inherit from it.
		<p>
		If this is `true`, inherit() will be called on all children that belongs to this `Entity`
		@see Entity#getProperty(unsigned int)
		*/
		const Byte ENTITY_PASS_DOWN = 5;
		/**
		Flag representing whether an Entity should inherit from it's parent.
		<p>
		If this is `true`, inherit() will NOT be called, regardless of the parent's `ENTITY_PASS_DOWN` setting.
		@see Entity#getProperty(unsigned int)
		*/
		const Byte ENTITY_IGNORE_PARENT = 6;
		const Byte ENTITY_STRETCH_X = 7;//
		const Byte ENTITY_STRETCH_Y = 8;//
		const Byte ENTITY_INHERIT_STRETCH_X = 9;//
		const Byte ENTITY_INHERIT_STRETCH_Y = 10;//

#ifdef _MACE_ENTITY_CUSTOM_DEFAULT_PROPERTIES
		const uint16_t ENTITY_DEFAULT_PROPERTIES = _MACE_ENTITY_CUSTOM_DEFAULT_PROPERTIES;
#else
		const uint16_t ENTITY_DEFAULT_PROPERTIES = 0b0110011000100110;
#endif
	}
}