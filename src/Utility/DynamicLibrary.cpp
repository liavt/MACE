/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Utility/DynamicLibrary.h>
#include <MACE/Core/System.h>

#ifdef MACE_POSIX
#	include <dlfcn.h>	
#endif//MACE_POSIX

namespace mc {
	DynamicLibrary DynamicLibrary::getRunningProcess() {
		DynamicLibrary runningProcess = DynamicLibrary();

#ifdef MACE_WINAPI
		runningProcess.dll = GetModuleHandle(nullptr);
#elif MACE_POSIX
		runningProcess.dll = dlopen(nullptr, 0);
#endif

		runningProcess.created = true;

		os::checkError(__LINE__, __FILE__, "Error retrieving current running process as dynamic library");

		return runningProcess;
	}

	DynamicLibrary::~DynamicLibrary() {
		if( isCreated() ) {
			destroy();
		}
	}

	DynamicLibrary::DynamicLibrary() {}

	DynamicLibrary::DynamicLibrary(const std::string & path) : DynamicLibrary(path.c_str()) {}

	DynamicLibrary::DynamicLibrary(const char * path) : DynamicLibrary() {
		init(path);
	}


	void DynamicLibrary::init(const std::string & path) {
		init(path.c_str());
	}

	void DynamicLibrary::init(const char * path) {
		if( isCreated() ) {
			throw InitializationFailedError("Can\'t reinitialize a DynamicLibrary object!");
		}

#ifdef MACE_WINAPI
		dll = LoadLibrary(path);
#elif defined(MACE_POSIX)
		dll = dlopen(path, RTLD_LAZY);
#endif

		if( dll == nullptr ) {
			os::checkError(__LINE__, __FILE__, "Attempt to load library " + std::string(path) + " returned a nullptr.");

			throw FileNotFoundError("Unable to load dynamic library: " + std::string(path));
		}

		created = true;

		os::checkError(__LINE__, __FILE__, "Error creating dynamic library at " + std::string(path));
	}


	void DynamicLibrary::destroy() {
		if( !isCreated() ) {
			throw AssertionFailedError("Can't destroy an uncreated DynamicLibrary");
		}

#ifdef MACE_WINAPI
		FreeLibrary(dll);
#elif defined(MACE_POSIX)
		dlclose(dll);
#endif

		os::checkError(__LINE__, __FILE__, "Error destroying dynamic library");
	}

	void * DynamicLibrary::getFunction(const std::string & name) {
		return getFunction(name.c_str());
	}

	void * DynamicLibrary::getFunction(const char * name) {
#ifdef MACE_WINAPI
		void* extractedSymbol = GetProcAddress(dll, name);
#elif defined(MACE_POSIX)
		void* extractedSymbol = dlsym(dll, name);
#endif

		if( extractedSymbol == nullptr ) {
			os::checkError(__LINE__, __FILE__, "Attempt to load symbol " + std::string(name) + " returned a nullptr.");

			throw NullPointerError("Unable to load symbol from dynamic library: " + std::string(name));
		}

		os::checkError(__LINE__, __FILE__, "Error extracting symbol " + std::string(name) + " from dynamic library");

		return extractedSymbol;
	}

	bool DynamicLibrary::isCreated() const {
		return created;
	}
}//mc