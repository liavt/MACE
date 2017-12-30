/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

#define MACE_DEBUG 1
#define MACE_EXPOSE_ALL 1
#include <MACE/MACE.h>

#define CATCH_CONFIG_RUNNER
#ifdef MACE_WINDOWS
#	define CATCH_CONFIG_WINDOWS_CRTDBG 1
#endif
#include <Catch.hpp>

#include <exception>
#include <iostream>


void onUnexpected[[noreturn]](){
	std::cerr << "Unexpected exception occured" << std::endl;
	std::abort();
}

void onTerminate[[noreturn]](){
	std::cerr << "An unhandled exception occured" << std::endl;
	std::abort();
}

int main(int argc, char* const argv[]) {
	try {
		std::set_unexpected(&onUnexpected);
		std::set_terminate(&onTerminate);

		//const int? constant? get it?
		const int result = Catch::Session().run(argc, argv);

		return result;
	} catch( const std::exception& e ) {
		mc::Error::handleError(e);
		return -1;
	} catch (...) {
		std::cerr << "An unknown exception occured";
		return -1;
	}

	return -1;
}