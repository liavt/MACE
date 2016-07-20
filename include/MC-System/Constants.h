#pragma once
#define __MACE true //this will be true if MACE is included

#define _MACE_TO_STRING(s) #s

#ifndef __cplusplus 
	#error A C++ compiler is required!
#endif 

#include <cstdint>


namespace mc {
	/**
	Primitive type guaranteed to be equal to 8 bits (1 byte). The definition here is for clarity.
	*/
	using Byte = uint8_t;

	/**
	Type representing an object's size.
	<p>
	The definition here is for clarity, as it makes code more self documenting. Imagine this:
	{@code
		unsigned int getSize()
	}
	By using this definition, that code becomes:
	{@code
		Size getSize()
	}
	Not only is it shorter, but it makes it more obvious, making documentation easier.
	@see Index
	*/
	using Size = unsigned int;

	/**
	Type representing an index in an array. 
	<p>
	The definition here is for clarity, and it makes code more obvious and self-documenting. Imagine this:
	{@code 
		removeObject(unsigned int)
	}
	or
	{@code 
		getObject(unsigned int)
	}
	`unsigned int` is ambigious! What if the object is an `unsigned int`? With this definition, it becomes much more clear:
	{@code
		removeObject(Index)

		getObject(Index)
	}

	@see Size
	*/
	using Index = unsigned int;

}