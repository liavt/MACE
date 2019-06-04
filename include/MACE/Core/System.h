/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__CORE_SYSTEM_H
#define MACE__CORE_SYSTEM_H

#include <MACE/Core/Constants.h>

//basically pushing and popping the macro __STDC_LIB_EXT1__
#ifdef __STDC_LIB_EXT1__
//this entire mechanism below is to make sure that defining __STDC_WANT_LIB_EXT1__ doesnt cause any issues
#	ifdef __STDC_WANT_LIB_EXT1__
#		define MACE__STDC_WAS_DEFINED __STDC_WANT_LIB_EXT1__
#	endif

#	define __STDC_WANT_LIB_EXT1__ 1
#endif//__STDC_LIB_EXT1__

#include <ctime>
#include <cstddef>

#ifdef __STDC_LIB_EXT1__
#	ifdef MACE__STDC_WAS_DEFINED
#		define __STDC_WANT_LIB_EXT1__ MACE__STDC_WAS_DEFINED
#	else
#		undef __STDC_WANT_LIB_EXT1__
#	endif

#	undef MACE__STDC_WAS_DEFINED
#endif//__STDC_LIB_EXT1__

#include <string>
#include <ostream>

#ifdef MACE_MSVC
#	include <intrin.h>
#	pragma intrinsic(_bittest)
#endif

namespace mc {

	/**
	Namespace with functions related to the operating system like keyboard input, mouse, and windows. Also contains cross-platform
	safer versions of many std functions.
	*/
	namespace os {
		std::tm* localtime(std::tm* result, const std::time_t* time) MACE_EXPECTS(result != nullptr && time != nullptr) MACE_ENSURES(ret, ret != nullptr);
		std::tm* gmtime(std::tm* result, const std::time_t* time) MACE_EXPECTS(result != nullptr && time != nullptr) MACE_ENSURES(ret, ret != nullptr);
		char* ctime(char* buffer, std::size_t bufSize, const std::time_t* time) MACE_EXPECTS(buffer != nullptr && time != nullptr) MACE_ENSURES(ret, ret != nullptr);
		char* asctime(char* buffer, std::size_t bufSize, const std::tm* time) MACE_EXPECTS(buffer != nullptr && time != nullptr) MACE_ENSURES(ret, ret != nullptr);

		FILE* fopen(FILE** result, const char* MACE_RESTRICT filename, const char* MACE_RESTRICT mode) MACE_EXPECTS(result != nullptr && filename != nullptr && mode != nullptr) MACE_ENSURES(ret, ret != nullptr);

		std::size_t mbsrtowcs(std::size_t* returnValue, wchar_t* wcstr, std::size_t sizeInWords, const char** mbstr, std::size_t count, mbstate_t* mbstate) MACE_EXPECTS(returnValue != nullptr && wcstr != nullptr && mbstr != nullptr && mbstate != nullptr);
		std::size_t wcstombs(std::size_t* returnValue, char* dst, std::size_t sizeInWords, const wchar_t* src, const std::size_t length) MACE_EXPECTS(returnValue != nullptr && dst != nullptr && src != nullptr);

		//the reason it is called assertion and not assert is because on some platforms, assert is a macro and that conflicts with the function declaration
		void assertion(const bool cond, const std::string& message) MACE_EXPECTS(!message.empty());
		void assertion(const bool cond, const char* message = "Assertion failed") MACE_EXPECTS(message != nullptr);

		void wait(const unsigned long long int ms);

		template<typename T>
		inline bool bittest(const T& val, const T& pos) {
#ifdef MACE_MSVC
			return _bittest(&static_cast<long>(val), static_cast<long>(pos));
#else
			return ((val) & (1 << (pos)));
#endif
		}

		std::wstring toWideString(const std::string& s);
		std::string toNarrowString(const std::wstring& s);

		template<typename SizeType = Size, class T, SizeType N>
		inline MACE_CONSTEXPR SizeType getArraySize(const T(&)[N]) noexcept {
			return N;
		}

		const char* strerror(char* buf, std::size_t bufsize, int errnum) MACE_EXPECTS(buf != nullptr);

		void clearError(const unsigned int lineNumber = 0, const char* filename = "Unknown file") MACE_EXPECTS(filename != nullptr);
		void checkError(const unsigned int lineNumber = 0, const char* filename = "Unknown file", const std::string message = "Unknown message") MACE_EXPECTS(filename != nullptr && !message.empty());

		void pause();

		enum class ConsoleColor {
			DEFAULT,
			WHITE,
			BLACK,
			BLUE,
			LIGHT_BLUE,
			GREEN,
			LIGHT_GREEN,
			CYAN,
			LIGHT_CYAN,
			RED,
			LIGHT_RED,
			MAGENTA,
			LIGHT_MAGENTA,
			YELLOW,
			LIGHT_YELLOW,
			GRAY,
			LIGHT_GRAY
		};

		/**
		@bug Background doesn't work on windows
		*/
		MACE_NODISCARD std::string consoleColor(const ConsoleColor& foreground = ConsoleColor::DEFAULT, const ConsoleColor& background = ConsoleColor::DEFAULT);
	}//os
}//mc

#endif//MACE_CORE_SYSTEM_H