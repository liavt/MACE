/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Core/Error.h>
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
			std::cerr << typeid(e).name();
			std::cerr << " occured:" << std::endl << "\t" << e.what() << std::endl;

#ifdef MACE_DEBUG
			const Error* err = dynamic_cast<const Error*>(&e);
			if (err != nullptr) {
				std::cerr << "\t[ " << err->getLine() << " @ " << err->getFile() << " ]" << std::endl;
			}
#endif
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

		dump << " MACE encountered an error and had to terminate the application." << std::endl;
		dump << "====ERROR DETAILS====" << std::endl;
		dump << "Type:" << std::endl << '\t' << typeid(e).name() << std::endl;
		dump << "Message:" << std::endl << '\t' << e.what() << std::endl;
		const Error* err = dynamic_cast<const Error*>(&e);
		if (err != nullptr) {
			dump << "Line: " << std::endl << '\t' << err->getLine() << std::endl;
			dump << "File: " << std::endl << '\t' << err->getFile() << std::endl;
		}
		dump << std::endl;
		dump << "====MACE DETAILS====" << std::endl;
		dump << "Module Handler Size:" << std::endl << '\t' << MACE::numberOfModules() << std::endl;
		dump << "Modules:";
		for (Index i = 0; i < MACE::numberOfModules(); ++i) {
			const Module* m = MACE::getModule(i);

			dump << std::endl << '\t' << m->getName() << " (" << typeid(*m).name() << ')';
		}
		//we need to flush it as well as newline. endl accomplishes that
		dump << std::endl;
		dump << "Flags:" << std::endl << '\t';
		dump << "DESTROYED - " << MACE::getFlag(MACE::DESTROYED) << std::endl << '\t';
		dump << "INIT - " << MACE::getFlag(MACE::INIT) << std::endl << '\t';
		dump << "STOP_REQUESTED - " << MACE::getFlag(MACE::STOP_REQUESTED);

		dump << std::endl << std::endl;

		//the strcmp checks if the macro is defined. if the name is different from it expanded, then it is a macro. doesnt work if a macro is defined as itself, but that shouldnt happen
#define MACE__CHECK_MACRO(name) if(std::strcmp("" #name ,MACE_STRINGIFY_NAME(name))){dump << "Yes";}else{dump << "No";}dump<<std::endl;
		dump << "====COMPILATION DETAILS====" << std::endl;
		dump << "Tests:" << std::endl << '\t';
		MACE__CHECK_MACRO(MACE_TESTS);
		dump << "Demos:" << std::endl << '\t';
		MACE__CHECK_MACRO(MACE_DEMOS);

		dump << std::endl;

		dump << "Include Directory:" << std::endl << '\t' << MACE_STRINGIFY_DEFINITION(MACE_INCLUDES) << std::endl;
		dump << "Source Directory:" << std::endl << '\t' << MACE_STRINGIFY_DEFINITION(MACE_DIRECTORY) << std::endl;
		dump << "System Name:" << std::endl << '\t' << MACE_STRINGIFY_DEFINITION(MACE_SYSTEM_NAME) << std::endl;
		dump << "System Version:" << std::endl << '\t' << MACE_STRINGIFY_DEFINITION(MACE_SYSTEM_VERSION) << std::endl;
		dump << "Processor:" << std::endl << '\t' << MACE_STRINGIFY_DEFINITION(MACE_PROCESSOR_NAME) << std::endl;

		dump << std::endl;

		dump << "OpenCV:" << std::endl << '\t';
		MACE__CHECK_MACRO(MACE_OPENCV);
		dump << "X11:" << std::endl << '\t';
		MACE__CHECK_MACRO(MACE_X11);
		dump << "POSIX:" << std::endl << '\t';
		MACE__CHECK_MACRO(MACE_POSIX);
		dump << "WinAPI:" << std::endl << '\t';
		MACE__CHECK_MACRO(MACE_WINAPI);

		dump << std::endl;

		dump << "Platform:" << std::endl << '\t';
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

		dump << "Endianness:" << std::endl << '\t';
#ifdef MACE_BIG_ENDIAN
		dump << "Big Endian";
#elif defined(MACE_LITTLE_ENDIAN)
		dump << "Little Endian";
#else
		dump << "Unknown Endianness";
#endif
		dump << std::endl;
		dump << "Static Libs:" << std::endl << '\t';
		MACE__CHECK_MACRO(MACE_STATIC_LIBS);
		dump << "Pointer Size:" << std::endl << '\t' << MACE_STRINGIFY_DEFINITION(MACE_POINTER_SIZE) << std::endl;
		dump << "Architecture Bitness:" << std::endl << '\t';
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

		dump << "Dynamic Library Prefix:" << std::endl << '\t' << MACE_STRINGIFY_DEFINITION(MACE_DYNAMIC_LIBRARY_PREFIX) << std::endl;
		dump << "Dynamic Library Suffix:" << std::endl << '\t' << MACE_STRINGIFY_DEFINITION(MACE_DYNAMIC_LIBRARY_SUFFIX) << std::endl;

#if defined(MACE_MSVC)
#	pragma warning( pop )
#endif

		dump << "Compiler:" << std::endl << '\t';

#ifdef MACE_MSVC
		dump << "Visual Studio";
#	ifdef MACE_MSVC_CL
		dump << " CL";
#	elif defined(MACE_MSVC_2005)
		dump << " 2005";
#	elif defined(MACE_MSVC_IDE)
		dump << " IDE";
#	endif

		dump << " (Version " << std::to_string(_MSC_VER) << ')';
#else
		dump << MACE_STRINGIFY_DEFINITION(MACE_COMPILER);
#endif
		dump << std::endl << "Version:" << std::endl << '\t';
		dump << MACE_STRINGIFY_DEFINITION(MACE_CMPILER_VERSION);

#undef MACE__CHECK_MACRO

		return dump.str();
	}

	void Error::setLogFileEnabled(const bool & writeLog) {
		writeToLog = writeLog;
	}

	void Error::setVerboseErrors(const bool & verbose) {
		verboseErrors = verbose;
	}

	Error::Error(const char * message, const unsigned int line, const std::string file) : Error(message, line, file.c_str()) {}

	Error::Error(const std::string message, const unsigned int line, const std::string file) : Error(message, line, file.c_str()) {}

	Error::Error(const char * message, const unsigned int l, const char * f) : std::runtime_error(message), line(l), file(f) {}

	Error::Error(const std::string message, const unsigned int l, const char * f) : std::runtime_error(message), line(l), file(f) {}

	unsigned int Error::getLine() const {
		return line;
	}

	const char * Error::getFile() const {
		return file;
	}
}
