/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__CORE_CONSTANTS_H
#define MACE__CORE_CONSTANTS_H

#include <MACE/Configure.h>

#ifndef __cplusplus
#	error A C++ compiler is required!
#endif//__cplusplus

#ifdef MACE_DEBUG
#	if MACE_DEBUG == 0 || MACE_DEBUG == false
#		undef MACE_DEBUG
#	endif//MACE_DEBUG == 0
#elif !defined(MACE_DEBUG)&&(defined(DEBUG) || (defined(_DEBUG)) || !defined(NDEBUG) || defined(MACE_DOXYGEN_PASS))
#	define MACE_DEBUG 1
#endif//elif

#if !defined(MACE_OPENCV)&&(defined(MACE_DOXYGEN_PASS)||(defined(CV_VERSION) && defined(CV_VERSION_MINOR) && defined(CV_VERSION_MINOR)))
#	define MACE_OPENCV 1
#endif

#ifdef MACE_WINDOWS
#	ifdef MACE_GNU
#		define MACE_FUNCTION_EXPORT __attribute__((dllexport))
#		define MACE_FUNCTION_EXPORT __attribute__((dllimport))
#	else
#		define MACE_FUNCTION_EXPORT __declspec(dllexport)
#		define MACE_FUNCTION_IMPORT __declspec(dllimport)
#	endif
#else
#	define MACE_FUNCTION_EXPORT __attribute__((visibility("default")))
#	define MACE_FUNCTION_IMPORT __attribute__((visibility("hidden")))
#endif

#define MACE_STRINGIFY(name) #name
#define MACE_STRINGIFY_NAME(name) "" #name
#define MACE_STRINGIFY_DEFINITION(name) "" MACE_STRINGIFY(name)

#ifdef MACE_DOXYGEN_PASS
#	define MACE_EXPOSE_WINAPI 1
#	define MACE_EXPOSE_POSIX 1
#endif

#include <cstdint>
#include <stdexcept>

namespace mc {
	/**
	Primitive type guaranteed to be equal to 8 bits (1 byte). The definition here is for clarity.
	@see VoidFunctionPtr
	@see Index
	@see Enum
	@see Size
	*/
	using Byte = std::uint8_t;

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
	@see Enum
	@see VoidFunctionPtr
	*/
	using Size = std::size_t;

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
	@see VoidFunctionPtr
	@see Enum
	*/
	using Index = Size;
	/**
	Type representing an int-based enum (such as GL_* constants)
	@see Index
	@see VoidFunctionPtr
	@see Size
	*/
	using Enum = unsigned int;

	/**
	Type representing a function that returns void and has no arguments.
	<p>
	Created for clarity and for use in callbacks
	@see Index
	@see Size
	@see Enum
	*/
	typedef void(*VoidFunctionPtr)();
}

#endif//MACE_CORE_CONSTANTS_H
