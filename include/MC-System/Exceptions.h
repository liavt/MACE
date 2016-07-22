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
}