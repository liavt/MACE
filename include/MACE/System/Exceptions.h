/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#include <stdexcept>

namespace mc {

	/**
	Superclass that all exceptions in MACE extend.
	*/
	class Exception : public std::runtime_error{
	public:
		/**
		Stops MACE and prints an exception to console accordingly. This should be used every time a fatal exception is thrown.
		*/
		static void handleException [[ noreturn ]](const std::exception& e);

		using std::runtime_error::runtime_error;
		
	};


	/**
	Thrown when a pointer is equal to NULL 
	*/
	using NullPointerException = Exception;


	/**
	Thrown when an assertion fails.
	@see System#assertModule(std::string)
	*/
	using AssertionError = Exception;

	/**
	Thrown when an object wasn't initializaed or initializtion failed
	*/
	using InitializationError = Exception;

	/**
	Thrown when a function looks for an object, but doesn't find it.
	*/
	using ObjectNotFoundInArrayException = Exception;
	/**
	Thrown when an index is provided for an array, but it is outside the valid bounds of the array.
	*/
	using IndexOutOfBoundsException = Exception;
	/**
	Thrown when something goes wrong while doing math, like dividing by zero
	*/
	using ArithmeticException = Exception;

}