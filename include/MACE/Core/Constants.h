/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_CORE_CONSTANTS_H
#define MACE_CORE_CONSTANTS_H

#include <MACE/Core/Configure.h>

#ifndef __cplusplus
#	error A C++ compiler is required!
#endif//__cplusplus

#ifdef MACE_DEBUG
#	if MACE_DEBUG == 0 || MACE_DEBUG == false
#		undef MACE_DEBUG
#	endif//MACE_DEBUG == 0
#elif defined(DEBUG) || (defined(_DEBUG)) || !defined(NDEBUG) || defined(MACE_DOXYGEN_PASS)
#	define MACE_DEBUG true
#endif//elif

#if defined(MACE_DOXYGEN_PASS)||(defined(CV_VERSION) && defined(CV_VERSION_MINOR) && defined(CV_VERSION_MINOR))
#	define MACE_OPENCV true
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

	/*Errors*/

	/**
	Superclass that all exceptions in MACE extend.
	*/
	class Error: public std::runtime_error {
	public:
		/**
		Stops MACE and prints an exception to console accordingly. This should be used every time a fatal exception is thrown.
		*/
		static void handleError[[noreturn]](const std::exception& e);

		using std::runtime_error::runtime_error;

		Error() noexcept = default;
		~Error() noexcept = default;

		void handle[[noreturn]]();
	};

#define _MACE_ERROR(name) class name##Error : public Error{public: using Error::Error;};

	/**
	Thrown when a pointer is equal to NULL
	*/
	_MACE_ERROR(NullPointer);

	/**
	Thrown when an assertion fails.
	@see MACE::assertModule(std::string)
	*/
	_MACE_ERROR(AssertionFailed);

	/**
	Thrown when an object wasn't initializaed or initializtion failed
	*/
	_MACE_ERROR(InitializationFailed);

	/**
	Thrown when an error occured trying to read or write an image
	*/
	_MACE_ERROR(BadImage);

	/**
	Thrown when an error occured trying to read or write a sound file
	*/
	_MACE_ERROR(BadSound);

	/**
	Thrown when a function looks for an object, but doesn't find it.
	*/
	_MACE_ERROR(ObjectNotFound);

	/**
	Thrown when something is of the wrong type
	*/
	_MACE_ERROR(InvalidType);

	/**
	Thrown when an index is provided for an array, but it is outside the valid bounds of the array.
	*/
	_MACE_ERROR(IndexOutOfBounds);
	/**
	Thrown when something goes wrong while doing math, like dividing by zero
	*/
	_MACE_ERROR(InvalidArithmetic);

	/**
	Thrown when something relate to Freetype or fonts fails
	*/
	_MACE_ERROR(Font);

	/**
	Thrown when OpenGL fails or errors
	@see ShaderError
	*/
	_MACE_ERROR(OpenGL);

	/**
	Thrown when a Shader throws an exception, such as a failed compilation.
	@see OpenGLError
	*/
	_MACE_ERROR(Shader);

	/**
	Thrown when a Framebuffer fails to be created, or throws an error
	@see gfx::ogl::Framebuffer
	@see OpenGLError
	*/
	_MACE_ERROR(Framebuffer);

	/**
	`Exception` thrown when a syntax error is encountered, or #error is called. The filename and line number will be in message.
	*/
	_MACE_ERROR(Preprocessor)
#undef _MACE_ERROR
}

#endif
