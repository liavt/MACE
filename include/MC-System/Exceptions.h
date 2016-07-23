/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#include <stdexcept>

namespace mc {
	/**
	Thrown when an assertion fails.
	@see System#assertModule(std::string)
	*/
	struct AssertionError : public std::runtime_error {
		/**
		Constructs the error with the specified message.
		@param c Message in the form of `char[]`
		*/
		explicit AssertionError(const char* c="No message was given") : runtime_error(c) {};
		/**
		Constructs the error with the specified message.
		@param c Message in the form of `std::string`
		*/
		explicit AssertionError(const std::string c = "No message was given") : runtime_error(c) {};
	};	
	/**
	Thrown when an object wasn't initializaed or initializtion failed
	*/
	struct InitializationError : public std::runtime_error {
		/**
		Constructs the error with the specified message.
		@param c Message in the form of `char[]`
		*/
		explicit InitializationError(const char* c = "No message was given") : runtime_error(c) {};
		/**
		Constructs the error with the specified message.
		@param c Message in the form of `std::string`
		*/
		explicit InitializationError(const std::string c = "No message was given") : runtime_error(c) {};
	};
	/**
	Thrown when a function looks for an object, but doesn't find it.
	*/
	struct ObjectNotFoundInArray : public std::runtime_error {
		/**
		Constructs the error with the specified message.
		@param c Message in the form of `char[]`
		*/
		explicit ObjectNotFoundInArray(const char* c = "No message was given") : runtime_error(c) {};
		/**
		Constructs the error with the specified message.
		@param c Message in the form of `std::string`
		*/
		explicit ObjectNotFoundInArray(const std::string c = "No message was given") : runtime_error(c) {};
	};
	/**
	Thrown when an index is provided for an array, but it is outside the valid bounds of the array.
	*/
	struct IndexOutOfBounds : public std::runtime_error {
		/**
		Constructs the error with the specified message.
		@param c Message in the form of `char[]`
		*/
		explicit IndexOutOfBounds(const char* c = "No message was given") : runtime_error(c) {};
		/**
		Constructs the error with the specified message.
		@param c Message in the form of `std::string`
		*/
		explicit IndexOutOfBounds(const std::string c = "No message was given") : runtime_error(c) {};
	};
	/**
	Thrown when something goes wrong while doing math, like dividing by zero
	*/
	struct ArithmeticError : public std::runtime_error {
		/**
		Constructs the error with the specified message.
		@param c Message in the form of `char[]`
		*/
		explicit ArithmeticError(const char* c = "No message was given") : runtime_error(c) {};
		/**
		Constructs the error with the specified message.
		@param c Message in the form of `std::string`
		*/
		explicit ArithmeticError(const std::string c = "No message was given") : runtime_error(c) {};
	};
}