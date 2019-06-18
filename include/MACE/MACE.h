/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__MACE_H
#define MACE__MACE_H

#include <MACE/Core/Core.h>
#include <MACE/Utility/Utility.h>
#include <MACE/Graphics/Graphics.h>

/**
Namespace for everything in MACE. This includes constants, typedefs, tests, classes, and variables.
<p>
All macros used by MACE are prefixed with `MACE_`. Internal macros which should not be used are prefixed with `MACE__`
<p>
No symbol in `mc` conflict directly with symbols found in `std` (C++11) or any dependency in MACE.
<p>
It is usually safe to type `using namespace mc`. However, be weary that this may create name conflicts with external libraries.

@note No MACE header file will bring all of `mc` into the global namespace.
@todo Add a proper logger
*/
namespace mc {
	/**
	Namespace which contains various internal classes and utilites. DO NOT use anything from this namespace
	unless you are developing MACE itself. Symbols in this namespace are not guarenteed to be the same
	throughout versions and may not be completely documented. Additionally, they may contain less error
	checking as they are only intended to be used internally.
	@internal
	*/
	namespace internal {
	}

	//the following functions are for use with runtime link libraries to verify the MACE version
	//they have a strange naming convention because name mangling is disabled and there has to be an
	//easy conflict-free way to access these functions
	extern "C" {
		MACE_FUNCTION_EXPORT inline MACE_CONSTEXPR unsigned int mcGetVersionMajor() {
			return MACE_VERSION_MAJOR;
		}

		MACE_FUNCTION_EXPORT inline MACE_CONSTEXPR unsigned int mcGetVersionMinor() {
			return MACE_VERSION_MINOR;
		}

		MACE_FUNCTION_EXPORT inline MACE_CONSTEXPR unsigned int mcGetVersionPatch() {
			return MACE_VERSION_PATCH;
		}

		MACE_FUNCTION_EXPORT inline MACE_CONSTEXPR const char* mcGetVersionString() {
			return MACE_STRINGIFY(MACE_VERSION);
		}

		MACE_FUNCTION_EXPORT inline MACE_CONSTEXPR bool mcIsDebugBuild() {
#ifdef MACE_DEBUG
			return true;
#else
			return false;
#endif
		}
	}
}

#endif