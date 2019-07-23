/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Core/Error.h>
#include <MACE/Core/Instance.h>
#include <MACE/Core/System.h>
#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <typeinfo>
#include <fstream>
#include <cstring>

namespace mc {
	void handleError(const std::exception& e, Instance* instance) {
		if (instance != nullptr && instance->getFlag(Instance::VERBOSE_ERRORS)) {
			std::cerr << getErrorDump(e);
		} else {
			std::cerr << os::consoleColor(os::ConsoleColor::RED) << "Exception occured: ";
			std::cerr << os::consoleColor(os::ConsoleColor::LIGHT_RED) << e.what() << '\n';
			//reset console color to default
			std::cerr << os::consoleColor() << std::flush;
		}

		if (instance != nullptr) {
			if (instance->getFlag(Instance::WRITE_ERRORS_TO_LOG)) {
				std::ofstream logFile;
				logFile.open("err.log", std::ofstream::out | std::ofstream::trunc);
				logFile << getErrorDump(e);
				logFile.close();
			}

			instance->requestStop();
		}

#ifdef MACE_DEBUG
		if (instance != nullptr) {
			throw e;
		}
#endif
		std::exit(EXIT_FAILURE);
	}

	void handleError(const std::exception& e, Instance& instance) {
		handleError(e, &instance);
	}

	std::string getErrorDump(const std::exception& e, const Instance* instance) {
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
		dump << e.what() << std::endl;
		dump << std::endl;
		if (instance != nullptr) {
			dump << "====MACE DETAILS====" << std::endl;
			dump << "Instance Size:" << std::endl << '\t' << instance->size() << std::endl;
			dump << "Modules:";
			for (auto val : instance->getModules()) {
				const ModulePtr m = val.second;

				dump << std::endl << '\t' << m->getName();
			}
			//we need to flush it as well as newline. endl accomplishes that
			dump << std::endl;
			dump << "Flags:" << std::endl << '\t';
			dump << "DESTROYED - " << instance->getFlag(Instance::DESTROYED) << std::endl << '\t';
			dump << "INIT - " << instance->getFlag(Instance::INIT) << std::endl << '\t';
			dump << "STOP_REQUESTED - " << instance->getFlag(Instance::STOP_REQUESTED);

			dump << std::endl << std::endl;
		}

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

		dump << "Dynamic Library Prefix:" << std::endl << '\t' << MACE_STRINGIFY_DEFINITION("" MACE_DYNAMIC_LIBRARY_PREFIX) << std::endl;
		dump << "Dynamic Library Suffix:" << std::endl << '\t' << MACE_STRINGIFY_DEFINITION("" MACE_DYNAMIC_LIBRARY_SUFFIX) << std::endl;

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
}
