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
		os::clearError(__LINE__, __FILE__);

		DynamicLibrary runningProcess = DynamicLibrary();

#ifdef MACE_WINAPI
		runningProcess.dll = GetModuleHandle(nullptr);

		constexpr char* errorMessage = "Handle to running process was nullptr";
#elif defined(MACE_POSIX)
		//the NULL macro is used instead of nullptr because dlopen accepts an argument of 0 specifically 
		//it needs to be an integer, not a pointer value.
		runningProcess.dll = dlopen(NULL, RTLD_LOCAL | RTLD_LAZY);

		char* systemError = dlerror();

		const char* errorMessage;

		if( systemError == nullptr ) {
			errorMessage = "Handle to running process was nullptr";
		} else {
			errorMessage = systemError;
		}
#endif

		if( runningProcess.dll == nullptr ) {
			throw NullPointerError("Internal Error: " + std::string(errorMessage));
		}

		runningProcess.created = true;

		os::checkError(__LINE__, __FILE__, "Error retrieving current running process as dynamic library: " + std::string(errorMessage));

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
		os::clearError(__LINE__, __FILE__);

		if( isCreated() ) {
			throw InitializationFailedError("Can\'t reinitialize a DynamicLibrary object!");
		}

#ifdef MACE_WINAPI
		dll = LoadLibrary(path);
#elif defined(MACE_POSIX)
		dll = dlopen(path, RTLD_LAZY);
#endif

#ifdef MACE_POSIX
		char* systemError = dlerror();

		const char* errorMessage;

		if( systemError == nullptr ) {
			errorMessage = "Library handle was nullptr";
		} else {
			errorMessage = systemError;
		}
#else
		constexpr char* errorMessage = "Library handle was nullptr";
#endif

		if( dll == nullptr ) {
			os::checkError(__LINE__, __FILE__, "Attempt to load library " + std::string(path) + " returned a nullptr: " + std::string(errorMessage));

			throw FileNotFoundError("Dynamic library not found: " + std::string(path) + ": " + std::string(errorMessage));
		}

		created = true;

		os::checkError(__LINE__, __FILE__, "Error creating dynamic library at " + std::string(path) + ": " + std::string(errorMessage));
	}


	void DynamicLibrary::destroy() {
		if( !isCreated() ) {
			throw AssertionFailedError("Can\'t destroy an uncreated DynamicLibrary");
		}

		if( dll == nullptr ) {
			throw NullPointerError("Can\'t destroy DynamicLibrary: internal handle was nullptr. Did initialization fail?");
		}

#ifdef MACE_WINAPI
		if( !FreeLibrary(dll) ) {
			os::checkError(__LINE__, __FILE__, "Destruction of dynamic library failed");

			throw AssertionFailedError("Destruction of dynamic library failed");
		}
#elif defined(MACE_POSIX)
		int result = dlclose(dll);

		if( result != 0 ) {
			const char* errorMessage = dlerror();

			os::checkError(__LINE__, __FILE__, "Failed to close dynamic library handle: Error code " + std::to_string(result) + ": " + std::string(errorMessage));

			throw AssertionFailedError("dlclose returned error code " + std::to_string(result) + ": " + std::string(errorMessage));
		}
#endif

		created = false;

		os::checkError(__LINE__, __FILE__, "Error destroying dynamic library");
	}

	void * DynamicLibrary::getFunction(const std::string & name) {
		return getFunction(name.c_str());
	}

	void * DynamicLibrary::getFunction(const char * name) {
		os::clearError(__LINE__, __FILE__);

		if( !isCreated() ) {
			throw InitializationFailedError("Can\'t load a function symbol from an unitialized dynamic library");
		}

#ifdef MACE_WINAPI
		void* extractedSymbol = GetProcAddress(dll, name);
#elif defined(MACE_POSIX)
		void* extractedSymbol = dlsym(dll, name);
#endif

		if( extractedSymbol == nullptr ) {
#ifdef MACE_POSIX
			const char* errorMessage = dlerror();
#else
			constexpr char* errorMessage = "Returned function pointer was null pointer";
#endif

			os::checkError(__LINE__, __FILE__, "Attempt to load symbol " + std::string(name) + " returned a nullptr: " + std::string(errorMessage));

			throw NullPointerError("Unable to load symbol from dynamic library: " + std::string(name) + ": " + std::string(errorMessage));
		}

		os::checkError(__LINE__, __FILE__, "Error extracting symbol " + std::string(name) + " from dynamic library");

		return extractedSymbol;
	}

	bool DynamicLibrary::isCreated() const {
		return created;
	}
}//mc
