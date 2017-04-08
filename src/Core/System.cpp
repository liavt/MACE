/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Core/System.h>
#include <MACE/Core/Constants.h>
#include <string>
#include <chrono>
#include <vector>
#include <thread>
#include <iostream>

#ifdef MACE_WINAPI
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#elif defined(MACE_POSIX)
#	include <unistd.h>
#	include <errno.h>
#	include <cstring>
#endif

namespace mc {
	namespace os {
		std::tm* localtime(std::tm * pointer, const std::time_t * time) {
			clearError(__LINE__, __FILE__);

#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if (errno_t status = localtime_s(pointer, time)) {
				throw AssertionFailedError("localtime_s errored with code " + std::to_string(status));
			}

			return pointer;
#elif defined(MACE_POSIX)
			localtime_r(time, pointer);

			checkError(__LINE__, __FILE__, "localtime_r failed");

			if (pointer == nullptr) {
				throw NullPointerError("localtime_r returned a nullptr");
			}

			return pointer;
#else
			//fallback to the built in localtime, which isn't thread safe
			return std::localtime(time);
#endif
		}

		std::tm* gmtime(std::tm * pointer, const std::time_t * time) {
			clearError(__LINE__, __FILE__);

#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if (errno_t status = gmtime_s(pointer, time)) {
				throw AssertionFailedError("gmtime_s errored with code " + std::to_string(status));
			}

			return pointer;
#elif defined(MACE_POSIX)
			gmtime_r(time, pointer);

			checkError(__LINE__, __FILE__, "gmtime_r failed");

			if (pointer == nullptr) {
				throw NullPointerError("gmtime_r returned a nullptr");
			}

			return pointer;
#else
			return std::gmtime(time);
#endif
		}

		char* ctime(char* buffer, std::size_t bufSize, const std::time_t* time) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if (errno_t status = ctime_s(buffer, bufSize, time)) {
				throw AssertionFailedError("ctime_s errored with code " + std::to_string(status));
			}

			return buffer;
#elif defined(MACE_POSIX)
			return ctime_r(time, buffer);
#else
			return std::ctime(time);
#endif
		}

		char* asctime(char* buffer, std::size_t bufSize, const std::tm* time) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if (errno_t status = asctime_s(buffer, bufSize, time)) {
				throw AssertionFailedError("asctime_s errored with code " + std::to_string(status));
			}

			return buffer;
#elif defined(MACE_POSIX)
			return asctime_r(time, buffer);
#else
			return std::asctime(time);
#endif
		}

		FILE * fopen(FILE ** result, const char * filename, const char * mode) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			mc::os::assertion(fopen_s(result, filename, mode) != 0, "fopen_s failed");

			return *result;
#else
			return std::fopen(filename, mode);
#endif
		}

		std::size_t mbsrtowcs(std::size_t * returnValue, wchar_t * wcstr, std::size_t sizeInWords, const char ** mbstr, std::size_t count, mbstate_t * mbstate) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if (errno_t status = mbsrtowcs_s(returnValue, wcstr, sizeInWords, mbstr, count, mbstate)) {
				throw AssertionFailedError("mbsrtowcs_s errored with code " + std::to_string(status));
			}

			return *returnValue;
#else
			return std::mbsrtowcs(wcstr, mbstr, count, mbstate);
#endif
		}

		std::size_t wcstombs(std::size_t * returnValue, char * dst, std::size_t sizeInWords, const wchar_t * src, std::size_t length) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if (errno_t status = wcstombs_s(returnValue, dst, sizeInWords, src, length)) {
				throw AssertionFailedError("mbsrtowcs_s errored with code " + std::to_string(status));
			}

			return *returnValue;
#else
			return std::wcstombs(dst, src, length);
#endif		
		}

		void assertion(const bool cond, const std::string & message) {
#ifdef MACE_DEBUG
			assertion(cond, message.c_str());
#endif
		}
		void assertion(const bool cond, const char * message) {
#ifdef MACE_DEBUG
			if (cond) {
				throw AssertionFailedError(message);
			}
#endif
		}

		void wait(const long long int ms) {
			std::this_thread::sleep_for(std::chrono::milliseconds(ms));
		}

		//thanks stack overflow for this function. the c++ library has no portable way to do this normally.
		std::wstring toWideString(const std::string & s) {
			clearError(__LINE__, __FILE__);

			const char * cs = s.c_str();
			std::size_t wn;
			wn = mc::os::mbsrtowcs(&wn, nullptr, 0, &cs, 0, nullptr);

			if (wn == std::size_t(-1)) {
				throw AssertionFailedError("Error in mbsrtowcs() with result " + std::to_string(errno));
			}

			std::vector<wchar_t> buf(wn + 1);
			std::size_t wn_again;
			wn = mc::os::mbsrtowcs(&wn_again, buf.data(), buf.size(), &cs, wn + 1, nullptr);

			if (wn_again == std::size_t(-1)) {
				throw AssertionFailedError("Error in mbsrtowcs() with result " + std::to_string(errno));
			}

			return std::wstring(buf.data(), wn);
		}

		std::string toNarrowString(const std::wstring & s) {
			clearError(__LINE__, __FILE__);

			const wchar_t * cs = s.c_str();
			std::size_t wn;
			wn = mc::os::wcstombs(&wn, nullptr, 0, cs, 0);

			if (wn == std::size_t(-1)) {
				throw AssertionFailedError("Error in wcstombs() with result " + std::to_string(errno));
			}

			std::vector<char> buf(wn + 1);
			std::size_t wn_again;
			wn = mc::os::wcstombs(&wn_again, buf.data(), buf.size(), cs, wn + 1);

			if (wn_again == std::size_t(-1)) {
				throw AssertionFailedError("Error in wcstombs() with result " + std::to_string(errno));
			}

			return std::string(buf.data(), wn);
		}

		void clearError(const int, const char*) {
			//os::checkError(line, file, "An unexpected error occured previously and was not caught correctly");
#ifdef MACE_WINAPI
			GetLastError();
#elif defined(MACE_POSIX)
			errno = 0;
#endif
		}

		void checkError(const int line, const char* file, const std::string errorMessage) {
#ifdef MACE_WINAPI
			DWORD error = GetLastError();
			if (error == 0) {
				//no error
				return;
			}

			LPSTR messageBuffer = nullptr;
			std::size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
											  nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, nullptr);

			std::string message(messageBuffer, size);

			LocalFree(messageBuffer);

			throw SystemError(std::to_string(line) + " in " + std::string(file) + ": " + errorMessage + ": Winapi threw error " + std::to_string(error) + " with message " + message);
#elif defined(MACE_POSIX)
			//POSIX man pages says to save the error before using it in case of a concurrent environment
			int error = errno;

			if (error == 0) {
				//no error
				return;
			}

			errno = 0;

			throw SystemError("Line " + std::to_string(line) + " in " + std::string(file) + ": " + errorMessage + ": " + std::strerror(error));
#endif
		}

		void pause() {
#ifdef MACE_WINDOWS
			system("pause");
#else
			std::cout << std::endl << "Press any key to continue...";
			std::cin.get();
#endif
		}
	}//os
}//mc
