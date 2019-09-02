/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#define MACE_DEBUG 1
#define MACE_EXPOSE_ALL 1
#include <MACE/MACE.h>

#define CATCH_CONFIG_RUNNER
#ifdef MACE_WINDOWS
#	define CATCH_CONFIG_WINDOWS_CRTDBG 1
#endif
#include <catch2/catch.hpp>

#include <exception>
#include <iostream>

void onTerminate MACE_NORETURN(){
	std::cerr << "An unhandled exception occured" << std::endl;
	std::abort();
}

int main(int argc, char* const argv[]) {
	try {
		std::set_terminate(&onTerminate);

		//const int? constant? get it?
		const int result = Catch::Session().run(argc, argv);

		return result;
	} catch( const std::exception& e ) {
		mc::handleError(e);
		return -1;
	} catch (...) {
		std::cerr << "An unknown exception occured";
		return -1;
	}

	return -1;
}