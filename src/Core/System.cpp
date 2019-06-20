/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Core/System.h>
#include <MACE/Core/Constants.h>
#include <MACE/Core/Error.h>
#include <string>
#include <chrono>
#include <vector>
#include <thread>
#include <iostream>
#include <sstream>

#ifdef MACE_WINAPI
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	undef WIN32_LEAN_AND_MEAN
#elif defined(MACE_POSIX)
#	include <unistd.h>
#	include <errno.h>
#	include <cstring>
#endif

namespace mc {
	namespace os {
		std::tm* localtime(std::tm* pointer, const std::time_t* time) {
			clearError(__LINE__, __FILE__);

#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if (const errno_t status = localtime_s(pointer, time)) {
				checkError(__LINE__, __FILE__, "Failed to use localtime_s: " + std::to_string(status));

				MACE__THROW(AssertionFailed, "localtime_s errored with code " + std::to_string(status));
			}

			return pointer;
#elif defined(MACE_POSIX)
			localtime_r(time, pointer);

			checkError(__LINE__, __FILE__, "localtime_r failed");

			if (pointer == nullptr) {
				checkError(__LINE__, __FILE__, "Failed to use localtime_r");

				MACE__THROW(NullPointer, "localtime_r returned a nullptr");
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
			if (const errno_t status = gmtime_s(pointer, time)) {
				checkError(__LINE__, __FILE__, "Failed to use gmtime_s: " + std::to_string(status));

				MACE__THROW(AssertionFailed, "gmtime_s errored with code " + std::to_string(status));
			}

			return pointer;
#elif defined(MACE_POSIX)
			gmtime_r(time, pointer);

			checkError(__LINE__, __FILE__, "gmtime_r failed");

			if (pointer == nullptr) {
				checkError(__LINE__, __FILE__, "Failed to use gmtime_r");

				MACE__THROW(NullPointer, "gmtime_r returned a nullptr");
			}

			return pointer;
#else
			return std::gmtime(time);
#endif
		}

		char* ctime(char* buffer, std::size_t bufSize, const std::time_t * time) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if (const errno_t status = ctime_s(buffer, bufSize, time)) {
				checkError(__LINE__, __FILE__, "Failed to use ctime_s: " + std::to_string(status));

				MACE__THROW(AssertionFailed, "ctime_s errored with code " + std::to_string(status));
			}

			return buffer;
#elif defined(MACE_POSIX)
			return ctime_r(time, buffer);
#else
			return std::ctime(time);
#endif
		}

		char* asctime(char* buffer, std::size_t bufSize, const std::tm * time) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if (const errno_t status = asctime_s(buffer, bufSize, time)) {
				checkError(__LINE__, __FILE__, "Failed to use asctime_s: " + std::to_string(status));

				MACE__THROW(AssertionFailed, "asctime_s errored with code " + std::to_string(status));
			}

			return buffer;
#elif defined(MACE_POSIX)
			return asctime_r(time, buffer);
#else
			return std::asctime(time);
#endif
		}

		FILE* fopen(FILE * *result, const char* filename, const char* mode) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if (const errno_t status = fopen_s(result, filename, mode)) {
				checkError(__LINE__, __FILE__, "Failed to use fopen_s: " + std::to_string(status));

				MACE__THROW(System, "fopen_s failed: " + std::to_string(status));
			}

			return *result;
#else
			return std::fopen(filename, mode);
#endif
		}

		std::size_t mbsrtowcs(std::size_t * returnValue, wchar_t* wcstr, std::size_t sizeInWords, const char** mbstr, std::size_t count, mbstate_t * mbstate) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if (const errno_t status = mbsrtowcs_s(returnValue, wcstr, sizeInWords, mbstr, count, mbstate)) {
				checkError(__LINE__, __FILE__, "Failed to use mbsrtowcs: " + std::to_string(status));

				MACE__THROW(AssertionFailed, "mbsrtowcs_s errored with code " + std::to_string(status));
			}

			return *returnValue;
#else
			return std::mbsrtowcs(wcstr, mbstr, count, mbstate);
#endif
		}

		std::size_t wcstombs(std::size_t * returnValue, char* dst, std::size_t sizeInWords, const wchar_t* src, std::size_t length) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if (const errno_t status = wcstombs_s(returnValue, dst, sizeInWords, src, length)) {
				checkError(__LINE__, __FILE__, "Failed to use wsctombs_s: " + std::to_string(status));

				MACE__THROW(AssertionFailed, "mbsrtowcs_s errored with code " + std::to_string(status));
			}

			return *returnValue;
#else
			return std::wcstombs(dst, src, length);
#endif		
		}

		void wait(const unsigned long long int ms) {
			std::this_thread::sleep_for(std::chrono::milliseconds(ms));
		}

		//thanks stack overflow for this function. the c++ library has no portable way to do this normally.
		std::wstring toWideString(const std::string & s) {
			clearError(__LINE__, __FILE__);

			const char* cs = s.c_str();
			std::size_t wn;
			wn = mc::os::mbsrtowcs(&wn, nullptr, 0, &cs, 0, nullptr);

			if (wn == std::size_t(-1)) {
				checkError(__LINE__, __FILE__, "Failed to use mbsrtowcs");

				MACE__THROW(AssertionFailed, "Error in mbsrtowcs(): Further error information unavailable");
			}

			std::vector<wchar_t> buf(wn + 1);
			std::size_t wn_again;
			wn = mc::os::mbsrtowcs(&wn_again, buf.data(), buf.size(), &cs, wn + 1, nullptr);

			if (wn_again == std::size_t(-1)) {
				checkError(__LINE__, __FILE__, "Failed to use mbsrtowcs");

				MACE__THROW(AssertionFailed, "Error in mbsrtowcs(): Further error information unavailable");
			}

			return std::wstring(buf.data(), wn);
		}

		std::string toNarrowString(const std::wstring & s) {
			clearError(__LINE__, __FILE__);

			const wchar_t* cs = s.c_str();
			std::size_t wn;
			wn = mc::os::wcstombs(&wn, nullptr, 0, cs, 0);

			if (wn == std::size_t(-1)) {
				checkError(__LINE__, __FILE__, "Failed to use wcstombs()");

				MACE__THROW(AssertionFailed, "Error in wcstombs(): Further error information unavailable");
			}

			std::vector<char> buf(wn + 1);
			std::size_t wn_again;
			wn = mc::os::wcstombs(&wn_again, buf.data(), buf.size(), cs, wn + 1);

			if (wn_again == std::size_t(-1)) {
				checkError(__LINE__, __FILE__, "Failed to use wcstombs()");

				MACE__THROW(AssertionFailed, "Error in wcstombs(): Further error information unavailable");
			}

			return std::string(buf.data(), wn);
		}

		const char* strerror(char* buf, std::size_t bufsize, int errnum) {
			//cant use checkError() or clearError() here as that would create an infinite recursive loop
			//clearError() and checkError() calls this function

#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if (const errno_t result = strerror_s(buf, bufsize, errnum) != 0) {
				return ("Failed to use strerror(): " + std::to_string(result) + ": errno was " + std::to_string(errno)).c_str();
			}

			return buf;
#else
			return std::strerror(errnum);
#endif
		}

		void clearError(const unsigned int, const char*) {
#ifdef MACE_WINAPI
			SetLastError(0);
#elif defined(MACE_POSIX)
			errno = 0;
#endif
		}

		void checkError(const unsigned int line, const char* file, const std::string& errorMessage) {
			//POSIX man pages says to save the error before using it in case of a concurrent environment
			const int error = errno;

			if (error == 0) {
				//no errno, check other systems for error

#ifdef MACE_WINAPI
				const DWORD lastError = GetLastError();
				if (lastError == 0) {
					//no error
					return;
				}

				LPSTR messageBuffer = nullptr;
				std::size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
												  nullptr, lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) & messageBuffer, 0, nullptr);

				std::string message(messageBuffer, size);

				LocalFree(messageBuffer);

				SetLastError(0);

				MACE__THROW_CUSTOM_LINE(System, errorMessage + ": Winapi threw error " + std::to_string(lastError) + " with message " + message, std::to_string(line), file);
#else
				return;
#endif
			}

			errno = 0;

			char buffer[128];

			MACE__THROW_CUSTOM_LINE(System, errorMessage + ": " + os::strerror(buffer, getArraySize(buffer), error), std::to_string(line), file);
		}

		void pause() {
#ifdef MACE_WINDOWS
			system("pause");
#else
			std::cout << std::endl << "Press any key to continue...";
			std::cin.get();
#endif
		}

		std::string consoleColor(const ConsoleColor & foreground, const ConsoleColor & background) {
#ifdef MACE_WINAPI
			Byte attribute = 0;
			const ConsoleColor colors[] = {background, foreground};

			for (unsigned int i = 0; i < 2; ++i) {
				attribute *= 15;
				switch (colors[i]) {
				case ConsoleColor::BLACK:
					attribute = 0;
					break;
				case ConsoleColor::BLUE:
					attribute = 1;
					break;
				case ConsoleColor::GREEN:
					attribute = 2;
					break;
				case ConsoleColor::CYAN:
					attribute = 3;
					break;
				case ConsoleColor::RED:
					attribute = 4;
					break;
				case ConsoleColor::MAGENTA:
					attribute = 5;
					break;
				case ConsoleColor::YELLOW:
					attribute = 6;
					break;
				case ConsoleColor::DEFAULT:
					MACE_FALLTHROUGH;
				case ConsoleColor::LIGHT_GRAY:
					MACE_FALLTHROUGH;
				default:
					attribute = 7;
					break;
				case ConsoleColor::GRAY:
					attribute = 8;
					break;
				case ConsoleColor::LIGHT_BLUE:
					attribute = 9;
					break;
				case ConsoleColor::LIGHT_GREEN:
					attribute = 10;
					break;
				case ConsoleColor::LIGHT_CYAN:
					attribute = 11;
					break;
				case ConsoleColor::LIGHT_RED:
					attribute = 12;
					break;
				case ConsoleColor::LIGHT_MAGENTA:
					attribute = 13;
					break;
				case ConsoleColor::LIGHT_YELLOW:
					attribute = 14;
					break;
				case ConsoleColor::WHITE:
					attribute = 15;
					break;
				}
			}

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(attribute));

			return "";
#elif defined(MACE_POSIX)
			std::ostringstream output;
			ConsoleColor colors[] = {foreground, background};

			for (unsigned int i = 0; i < 2; ++i) {
				unsigned int code;

				switch (colors[i]) {
				case ConsoleColor::DEFAULT:
				default:
					code = 39;
					break;
				case ConsoleColor::BLACK:
					code = 30;
					break;
				case ConsoleColor::RED:
					code = 31;
					break;
				case ConsoleColor::GREEN:
					code = 32;
					break;
				case ConsoleColor::YELLOW:
					code = 33;
					break;
				case ConsoleColor::BLUE:
					code = 34;
					break;
				case ConsoleColor::MAGENTA:
					code = 35;
					break;
				case ConsoleColor::CYAN:
					code = 36;
					break;
				case ConsoleColor::LIGHT_GRAY:
					code = 37;
					break;
				case ConsoleColor::GRAY:
					code = 90;
					break;
				case ConsoleColor::LIGHT_RED:
					code = 91;
					break;
				case ConsoleColor::LIGHT_GREEN:
					code = 92;
					break;
				case ConsoleColor::LIGHT_YELLOW:
					code = 93;
					break;
				case ConsoleColor::LIGHT_BLUE:
					code = 94;
					break;
				case ConsoleColor::LIGHT_MAGENTA:
					code = 95;
					break;
				case ConsoleColor::LIGHT_CYAN:
					code = 96;
					break;
				case ConsoleColor::WHITE:
					code = 97;
					break;
				}

				code += (i * 10);

				output << "\033[";
				output << code;
				output << "m";
			}

			return output.str();
#else
			//in case of unsupported system, just be safe and return nothing
			return "";
#endif
		}
	}//os
}//mc
