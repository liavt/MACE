/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Core/System.h>
#include <MACE/Core/Constants.h>
#include <string>

namespace mc {
	namespace os {
		std::tm* localtime(std::tm * pointer, const std::time_t * time) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if( errno_t status = localtime_s(pointer, time) ) {
				throw AssertionError("localtime_s errored with code " + std::to_string(status));
			}

			return pointer;
#elif defined(MACE_POSIX)
			localtime_r(time, pointer);
#else
			//fallback to the built in localtime, which isn't thread safe
			return std::localtime(time);
#endif
		}

		std::tm* gmtime(std::tm * pointer, const std::time_t * time) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if( errno_t status = gmtime_s(pointer, time) ) {
				throw AssertionError("gmtime_s errored with code " + std::to_string(status));
			}

			return pointer;
#elif defined(MACE_POSIX)
			gmtime_r(time, pointer);

			if( pointer == nullptr ) {
				throw NullPointerException("gmtime_r returned a nullptr");
			}

			return pointer;
#else
			return std::gmtime(time);
#endif
		}

		char* ctime(char* buffer, std::size_t bufSize, const std::time_t* time) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if( errno_t status = ctime_s(buffer, bufSize, time) ) {
				throw AssertionError("ctime_s errored with code " + std::to_string(status));
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
			if( errno_t status = asctime_s(buffer, bufSize, time) ) {
				throw AssertionError("asctime_s errored with code " + std::to_string(status));
			}

			return buffer;
#elif defined(MACE_POSIX)
			return asctime_r(time, buffer);
#else
			return std::asctime(time);
#endif
		}

		std::size_t* mbsrtowcs(std::size_t * returnValue, wchar_t * wcstr, std::size_t sizeInWords, const char ** mbstr, std::size_t count, mbstate_t * mbstate) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if( errno_t status = mbsrtowcs_s(returnValue, wcstr, sizeInWords, mbstr, count, mbstate) ) {
				throw AssertionError("asctime_s errored with code " + std::to_string(status));
			}

			return returnValue;
#else
			return std::mbsrtowcs(wcstr, mbstr, count, mbstate);
#endif
		}
		void assert(const bool cond, const std::string & message) {
			assert(cond, message.c_str());
		}
		void assert(const bool cond, const char * message) {
#ifdef MACE_ERROR_CHECK
			if( cond ) {
				throw AssertionError(message);
			}
#endif
		}
	}//os
}//mc
