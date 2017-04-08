/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Core/Constants.h>
#include <MACE/Core/Module.h>
#include <MACE/Core/System.h>
#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <typeinfo>
#include <fstream>
#include <cstring>

namespace mc {
	namespace {
		bool writeToLog = false;
		bool verboseErrors = false;
	}

	void Error::handleError(const std::exception & e) {
		if (verboseErrors) {
			std::cerr << Error::getErrorDump(e);
		} else {
			std::cerr << "Error occured:\n\t" << e.what() << std::endl;
		}

		if (writeToLog) {
			std::ofstream logFile;
			logFile.open("err.log", std::ofstream::out | std::ofstream::trunc);
			logFile << Error::getErrorDump(e);
			logFile.close();
		}

		MACE::requestStop();

		throw e;
	}

	void Error::handle() {
		Error::handleError(*this);
	}

	std::string Error::getErrorDump(const std::exception & e) {
		std::stringstream dump;
		dump << "At ";

		{
			std::time_t time = std::time(nullptr);
			std::tm timeStruct;

			timeStruct = *os::localtime(&timeStruct, &time);

			char buffer[64];

			strftime(buffer, 64, "%b/%e/%Y %H:%M:%S", &timeStruct);

			dump << std::string(buffer);
		}

		dump << " MACE encountered an error and had to terminate the application.\n";
		dump << "====ERROR DETAILS====\n";
		dump << "Type:\n\t" << typeid(e).name() << "\n";
		dump << "Message:\n\t" << e.what() << "\n";
		dump << "\n";
		dump << "====MACE DETAILS====\n";
		dump << "Module Handler Size:\n\t" << MACE::numberOfModules() << "\n";
		dump << "Modules:";
		for (Index i = 0; i < MACE::numberOfModules(); ++i) {
			const Module* m = MACE::getModule(i);

			dump << "\n\t" << m->getName() << " (" << typeid(*m).name() << ")";
		}
		//we need to flush it as well as newline. endl accomplishes that
		dump << std::endl;
		dump << "Flags:\n\t";
		dump << "DESTROYED - " << MACE::getFlag(MACE::Flags::DESTROYED) << "\n\t";
		dump << "INIT - " << MACE::getFlag(MACE::Flags::INIT) << "\n\t";
		dump << "STOP_REQUESTED - " << MACE::getFlag(MACE::Flags::STOP_REQUESTED);

		dump << "\n\n";

		//the strcmp checks if the macro is defined. if the name is different from it expanded, then it is a macro. doesnt work if a macro is defined as itself, but that shouldnt happen
#define _MACE_CHECK_MACRO(name) if(std::strcmp("" #name ,MACE_STRINGIFY_NAME(name))){dump << "Yes";}else{dump << "No";}dump<<"\n";
		dump << "====COMPILATION DETAILS====\n";
		dump << "Tests:\n\t";
		_MACE_CHECK_MACRO(MACE_TESTS);
		dump << "Demos:\n\t";
		_MACE_CHECK_MACRO(MACE_DEMOS);

		dump << std::endl;

		dump << "Include Directory:\n\t" << MACE_STRINGIFY_DEFINITION(MACE_INCLUDES) << "\n";
		dump << "Source Directory:\n\t" << MACE_STRINGIFY_DEFINITION(MACE_DIRECTORY) << "\n";
		dump << "System Name:\n\t" << MACE_STRINGIFY_DEFINITION(MACE_SYSTEM_NAME) << "\n";
		dump << "System Version:\n\t" << MACE_STRINGIFY_DEFINITION(MACE_SYSTEM_VERSION) << "\n";
		dump << "Processor:\n\t" << MACE_STRINGIFY_DEFINITION(MACE_PROCESSOR_NAME) << "\n";

		dump << std::endl;

		dump << "OpenCV:\n\t";
		_MACE_CHECK_MACRO(MACE_OPENCV);
		dump << "X11:\n\t";
		_MACE_CHECK_MACRO(MACE_X11);
		dump << "POSIX:\n\t";
		_MACE_CHECK_MACRO(MACE_POSIX);
		dump << "WinAPI:\n\t";
		_MACE_CHECK_MACRO(MACE_WINAPI);

		dump << std::endl;

		dump << "Platform:\n\t";
#ifdef MACE_WINDOWS
		dump << "Windows";
#elif defined(MACE_OSX)
		dump << "OSX";
#elif defined(MACE_UNIX)
		dump << "Unix";
#else
		dump << "Unknown platform";
#endif

		dump << std::endl;

		dump << "Endianness:\n\t";
#ifdef MACE_BIG_ENDIAN
		dump << "Big Endian";
#elif defined(MACE_LITTLE_ENDIAN)
		dump << "Little Endian";
#else
		dump << "Unknown Endianness";
#endif
		dump << std::endl;
		dump << "Static Libs:\n\t";
		_MACE_CHECK_MACRO(MACE_STATIC_LIBS);
		dump << "Pointer Size:\n\t" << MACE_STRINGIFY_DEFINITION(MACE_POINTER_SIZE) << "\n";
		dump << "Architecture Bitness:\n\t";
#ifdef MACE_32_BIT
		dump << "32 Bit";
#elif defined(MACE_64_BIT)
		dump << "64 Bit";
#else
		dump << "Unknown Bitness";
#endif
		//we need to flush it as well as newline. endl accomplishes that
		dump << std::endl;

#if defined(MACE_MSVC)
#	pragma warning( push ) 
		//this warning is to say that a macro does not have enough parameters.
		//this can be raised on systems where MACE_DYNAMIC_LIBRARY_PREFIX is empty (cough cough Windows)
#	pragma warning( disable: 4003 ) 
#endif 

		dump << "Dynamic Library Prefix:\n\t" << MACE_STRINGIFY_DEFINITION(MACE_DYNAMIC_LIBRARY_PREFIX) << "\n";
		dump << "Dynamic Library Suffix:\n\t" << MACE_STRINGIFY_DEFINITION(MACE_DYNAMIC_LIBRARY_SUFFIX) << "\n";

#if defined(MACE_MSVC)
#	pragma warning( pop )
#endif

		dump << "Compiler:\n\t";

#ifdef MACE_MSVC
		dump << "Visual Studio";
#	ifdef MACE_MSVC_CL
		dump << " CL";
#	elif defined(MACE_MSVC_2005)
		dump << " 2005";
#	elif defined(MACE_MSVC_IDE)
		dump << " IDE";
#	endif

		dump << " (Version " << std::to_string(_MSC_VER) << ")";
#else
		dump << MACE_STRINGIFY_DEFINITION(MACE_COMPILER);
#endif

#undef _MACE_CHECK_MACRO

		return dump.str();
	}

	void Error::setLogFileEnabled(const bool & writeLog) {
		writeToLog = writeLog;
	}

	void Error::setVerboseErrors(const bool & verbose) {
		verboseErrors = verbose;
	}
}
