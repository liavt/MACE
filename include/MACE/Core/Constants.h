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

#define MACE_MAKE_VERSION_NUMBER(major,minor,patch) (((major % 100) * 10000000) + ((minor % 100) * 100000) + (patch % 100000))
#define MACE_MAKE_VERSION_STRING(major, minor, patch) major##.##minor##.##patch

//COMPILER SUPPORT
#ifdef __GNUC__
#	define MACE_GNU __GNUC__
#	if defined(__clang__) && defined(__clang_major__) && defined(__clang_minor__) && defined(__clang_patchlevel__)
#		define MACE_CLANG MACE_MAKE_VERSION_NUMBER(__clang_major__, __clang_minor__, __clang_patchlevel__)
#	elif defined(__GNUG__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
#		define MACE_GCC MACE_MAKE_VERSION_NUMBER(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#	elif defined(__MINGW32__) || defined(__MINGW64__)
#		define MACE_MINGW MACE_MAKE_VERSION_NUMBER(__MINGW64_VERSION_MAJOR, __MINGW64_VERSION_MINOR, 0)
#	elif defined(__ICC) || defined(__INTEL_COMPILER)
#		define MACE_INTEL __INTEL_COMPILER
#	else
#		error "Unable to detect GNU compiler version!"
#	endif
#elif defined(_MSC_VER)
#	define MACE_MSVC _MSC_FULL_VER
#elif defined(__BORLANDC__)
#	define MACE_BORLAND __BORLANDC__
#elif defined(__HP_cc) && defined(__HP_aCC)
#	define MACE_HP __HP_acc
#elif defined(__SUNPRO_C) && defined(__SUNPRO_CC)
#	define MACE_SOLARIS __SUNPRO_CC
#elif defined(__WATCOMC__)
#	define MACE_WATCOM __WATCOMC__
#elif defined(_CRAYC)
#	define MACE_CRAY _REVISION
#elif defined(sgi)&&defined(__sgi)
#	define MACE_MIPSPRO __COMPILER_VERSION
#elif defined(__PGIC__)
#	define MACE_PORTLAND MACE_MAKE_VERSION_NUMBER(__PGIC__, __PGIC_MINOR, __PGIC_PATCHLEVEL__)
#elif defined(__IMBC__) && defined(__IMBCC__)
#	define MACE_IBM __IMBCC__
#else
#	error "Unknown compiler detected!"
#endif

//checking if MACE should be compiled in debug mode
#ifdef MACE_DEBUG
#	if !MACE_DEBUG
#		undef MACE_DEBUG
#	endif//MACE_DEBUG == 0
#elif !defined(MACE_DEBUG)&&(defined(DEBUG) || (defined(_DEBUG)) || !defined(NDEBUG) || defined(MACE_DOXYGEN_PASS))
#	define MACE_DEBUG 1
#endif//elif

#ifdef MACE_DEBUG
#	define MACE_DEBUG_OPENGL 1
#	define MACE_DEBUG_CHECK_ARGS 1
#	define MACE_DEBUG_INTERNAL_ERRORS 1
#	define MACE_DEBUG_CHECK_NULLPTR 1
#endif

//checks for a C++ attribute in the form of [[attribute]]
#ifndef MACE_HAS_ATTRIBUTE
#	ifndef __has_cpp_attribute
#		define MACE_HAS_ATTRIBUTE(attr) false
#	else
#		define MACE_HAS_ATTRIBUTE(attr) __has_cpp_attribute(attr)
#	endif
#endif

//[[fallthrough]] attribute
#ifndef MACE_FALLTHROUGH
#	if MACE_HAS_ATTRIBUTE(fallthrough)
#		define MACE_FALLTHROUGH [[fallthrough]]
#	else
#		define MACE_FALLTHROUGH
#	endif
#endif

//used to mark a function or variable thats deprecated
#ifndef MACE_DEPRECATED
#	if MACE_HAS_ATTRIBUTE(deprecated)
#		define MACE_DEPRECATED(message) [[deprecated(message)]]
#	else
#		ifdef MACE_MSVC
#			define MACE_DEPRECATED(message) __declspec(deprecated(message))
#		elif defined(MACE_GNU)
#			define MACE_DEPRECATED(message)  __attribute__((deprecated))
#		else
#			define MACE_DEPRECATED(message)
#		endif
#	endif
#endif

//defining keywords defining function symbol export and import
#if !defined(MACE_FUNCTION_EXPORT)&&!defined(MACE_FUNCTION_IMPORT)
#	if MACE_HAS_ATTRIBUTE(dllexport) && MACE_HAS_ATTRIBUTE(dllimport)
#		define MACE_FUNCTION_EXPORT [[dllexport]]
#		define MACE_FUNCTION IMPORT [[dllimport]]
#	elif defined(MACE_WINDOWS)
#		ifdef MACE_GNU
#			define MACE_FUNCTION_EXPORT __attribute__((dllexport))
#			define MACE_FUNCTION_IMPORT __attribute__((dllimport))
#		else
#			define MACE_FUNCTION_EXPORT __declspec(dllexport)
#			define MACE_FUNCTION_IMPORT __declspec(dllimport)
#		endif
#	else
#		define MACE_FUNCTION_EXPORT __attribute__((visibility("default")))
#		define MACE_FUNCTION_IMPORT __attribute__((visibility("hidden")))
#	endif
#endif

//whether the specificed include is available
#ifndef MACE_HAS_INCLUDE
#	ifndef __has_include
#		define MACE_HAS_INCLUDE(incl) false
#	else
#		define MACE_HAS_INCLUDE(incl) __has_include(incl)
#	endif
#endif

//whether opencv interoptibility should be built
#ifdef MACE_OPENCV
#	if !MACE_OPENCV
#		undef MACE_OPENCV
#	endif
//if doxygen is running or opencv is detected, set MACE_OPENCV to 1 if it hasnt been defined already
#elif MACE_HAS_INCLUDE(<opencv2/opencv.hpp>)||defined(MACE__DOXYGEN_PASS)||(defined(CV_VERSION) && defined(CV_VERSION_MINOR) && defined(CV_VERSION_MINOR))
#	define MACE_OPENCV 1
#endif

//constexpr
#ifndef MACE_CONSTEXPR
#	if defined(__cpp_constexpr) && __cpp_constexpr >= 200704
#		define MACE_CONSTEXPR constexpr
#	else
#		define MACE_CONSTEXPR
#	endif
#endif

//static assert
//allows users to use a different static assert (such as boost's static asssert)
#ifndef MACE_STATIC_ASSERT
#	define MACE_STATIC_ASSERT(cond, message) static_assert( cond , message )
#endif

#define MACE_STRINGIFY(name) #name
#define MACE_STRINGIFY_NAME(name) "" #name
#define MACE_STRINGIFY_DEFINITION(name) "" MACE_STRINGIFY(name)

#define MACE_CONCAT(arg1, ...) arg1 ## __VA_ARGS__

//meaning doxygen is currently parsing this file
#ifdef MACE__DOXYGEN_PASS
#	define MACE_EXPOSE_ALL 1
#endif

#ifdef MACE_EXPOSE_ALL
#	define MACE_EXPOSE_WINAPI 1
#	define MACE_EXPOSE_POSIX 1
#	define MACE_EXPOSE_OPENGL 1
#	define MACE_EXPOSE_GLFW 1
#	define MACE_EXPOSE_OPENAL 1
#endif

//for std::uint8_t
#include <cstdint>
//for std::size_t
#include <cstddef>

namespace mc {
	/**
	Primitive type guaranteed to be at least equal to 8 bits (1 byte). The definition here is for clarity.
	@see Index
	@see Enum
	@see Size
	*/
	using Byte = std::uint_least8_t;

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
	@see Enum
	*/
	using Index = Size;
	/**
	Type representing an int-based enum (such as GL_* constants)
	@see Index
	@see Size
	*/
	using Enum = unsigned int;
}

#endif//MACE_CORE_CONSTANTS_H
