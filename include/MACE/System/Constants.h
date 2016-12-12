﻿/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_SYSTEM_CONSTANTS_H
#define MACE_SYSTEM_CONSTANTS_H

#define MACE_INCLUDED true //this will be true if MACE is included

#ifndef __cplusplus 
#	error A C++ compiler is required!
#endif 

#if !(defined(_WIN32) || defined(__APPLE__) || defined(__linux__) || defined(__unix__) || defined(_POSIX_VERSION))
#	warning "This is a system on which MACE has not been tested with. MACE may have undefined behavior."
#endif

#if (defined(DEBUG)) || (defined(_DEBUG))
#	define MACE_ERROR_CHECK
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
	@see VoidFunctionPtr
	@see Enum
	*/
	using Index = unsigned int;
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

	/*Exceptions*/

	/**
	Superclass that all exceptions in MACE extend.
	*/
	class Exception: public std::runtime_error {
	public:
		/**
		Stops MACE and prints an exception to console accordingly. This should be used every time a fatal exception is thrown.
		*/
		static void handleException[[noreturn]](const std::exception& e);

		using std::runtime_error::runtime_error;

		Exception() noexcept = default;
		~Exception() noexcept = default;
	};


	/**
	Thrown when a pointer is equal to NULL
	*/
	using NullPointerException = Exception;


	/**
	Thrown when an assertion fails.
	@see System#assertModule(std::string)
	*/
	using AssertionError = Exception;

	/**
	Thrown when an object wasn't initializaed or initializtion failed
	*/
	using InitializationError = Exception;

	/**
	Thrown when a function looks for an object, but doesn't find it.
	*/
	using ObjectNotFoundInArrayException = Exception;
	/**
	Thrown when an index is provided for an array, but it is outside the valid bounds of the array.
	*/
	using IndexOutOfBoundsException = Exception;
	/**
	Thrown when something goes wrong while doing math, like dividing by zero
	*/
	using ArithmeticException = Exception;
}

#endif