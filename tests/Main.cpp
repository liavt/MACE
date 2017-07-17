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

int main(int argc, char* const argv[]) {
	try {
		//constant? get it?
		const int result = Catch::Session().run(argc, argv);

		return result;
	} catch( const std::exception& e ) {
		mc::Error::handleError(e);
	}

	return -1;
}