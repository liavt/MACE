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
		void localtime(std::tm * pointer, const std::time_t * time) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if( errno_t status = localtime_s(pointer, time) ) {
				throw AssertionError("localtime_s errored with code " + std::to_string(status));
			}
#elif MACE_POSIX
			localtime_r(time, pointer);
#else
			//fallback to the built in localtime, which isn't thread safe
			*pointer = *std::localtime(time);

			if( pointer == nullptr ) {
				throw NullPointerException("localtime returned a nullptr");
			}
#endif
		}

		void gmtime(std::tm * pointer, const std::time_t * time) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if( errno_t status = gmtime_s(pointer, time) ) {
				throw AssertionError("gmtime_s errored with code " + std::to_string(status));
			}
#elif MACE_POSIX
			gmtime_r(time, pointer);

			if( pointer == nullptr ) {
				throw NullPointerException("gmtime_r returned a nullptr");
			}
#else
			*pointer = *std::gmtime(time);

			if( pointer == nullptr ) {
				throw NullPointerException("gmtime returned a nullptr");
			}
#endif
		}

		void ctime(char* buffer, rsize_t bufSize, const std::time_t* time) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if( errno_t status = ctime_s(buffer, bufSize, time) ) {
				throw AssertionError("ctime_s errored with code " + std::to_string(status));
			}
#elif MACE_POSIX
			*buffer = ctime_r(buffer, time);

			if( buffer == nullptr ) {
				throw NullPointerException("ctime_r returned a nullptr");
			}
#else
			*buffer = *std::ctime(time);

			if( buffer == nullptr ) {
				throw NullPointerException("ctime returned a nullptr");
			}
#endif
		}
		void asctime(char* buffer, std::size_t bufSize, const std::tm* time) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if( errno_t status = asctime_s(buffer, bufSize, time) ) {
				throw AssertionError("asctime_s errored with code " + std::to_string(status));
			}
#elif MACE_POSIX
			*buffer = asctime_r(time, buffer);

			if( buffer == nullptr ) {
				throw NullPointerException("asctime_r returned a nullptr");
			}
#else
			*buffer = *std::asctime(time);

			if( buffer == nullptr ) {
				throw NullPointerException("asctime returned a nullptr");
			}
#endif
		}

		void mbsrtowcs(std::size_t * returnValue, wchar_t * wcstr, std::size_t sizeInWords, const char ** mbstr, std::size_t count, mbstate_t * mbstate) {
#if defined(MACE_WINDOWS) || defined(__STDC_LIB_EXT1__)
			if( errno_t status = mbsrtowcs_s(returnValue, wcstr, sizeInWords, mbstr, count, mbstate) ) {
				throw AssertionError("asctime_s errored with code " + std::to_string(status));
			}
#else
			*returnValue = std::mbsrtowcs(wcstr, mbstr, count, mbstate);

			if( returnValue == nullptr ) {
				throw NullPointerException("mbsrtowcs returned a nullptr");
			}
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
