/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

#pragma once
#ifndef MACE_CORE_SYSTEM_H
#define MACE_CORE_SYSTEM_H

#define __STDC_WANT_LIB_EXT1__ 1
#include <ctime>
#include <cstddef>

#include <string>

namespace mc {

	/**
	Namespace with functions related to the operating system like keyboard input, mouse, and windows. Also contains cross-platform
	safer versions of many std functions.
	*/
	namespace os {
		std::tm* localtime(std::tm* result, const std::time_t* time);
		std::tm* gmtime(std::tm* result, const std::time_t* time);
		char* ctime(char* buffer, std::size_t bufSize, const std::time_t* time);
		char* asctime(char* buffer, std::size_t bufSize, const std::tm* time);

		FILE* fopen(FILE** result, const char* filename, const char* mode);

		std::size_t mbsrtowcs(std::size_t* returnValue, wchar_t* wcstr, std::size_t sizeInWords, const char** mbstr, std::size_t count, mbstate_t* mbstate);
		std::size_t wcstombs(std::size_t* returnValue, char* dst, std::size_t sizeInWords, const wchar_t* src, const std::size_t length);

		void assertion(const bool cond, const std::string& message);
		void assertion(const bool cond, const char* message = "Assertion failed");

		void wait(const long long int ms);

		std::wstring toWideString(const std::string& s);
		std::string toNarrowString(const std::wstring& s);

		template<class T, std::size_t n>
		inline constexpr std::size_t getArraySize(T(&)[n]) {
			return n;
		}

		void checkError(const int lineNumber = 0, const char* filename = "Unknown file", const std::string message = "Unknown message");

		void pause();
	}//os
}//mc

#endif//MACE_CORE_SYSTEM_H