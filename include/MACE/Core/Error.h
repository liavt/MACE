/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__CORE_ERROR_H
#define MACE__CORE_ERROR_H
#include <string>
#include <stdexcept>

namespace mc {
	//forward declaration for handleError();
	class Instance;

	/**
	Superclass that all exceptions in MACE extend.
	*/
	class Error: public std::runtime_error {
	public:
		static std::string getErrorDump(const std::exception& e, const Instance* i = nullptr);

		/**
		Stops MACE and prints an exception to console accordingly. This should be used every time a fatal exception is thrown.
		*/
		static void handleError[[noreturn]](const std::exception& e, Instance* i = nullptr);// i is not const because requestStop() will be called

		/**
		@copydoc Error::handleError(const std::exception& e, Instance*)
		*/
		static void handleError[[noreturn]](const std::exception& e, Instance& i);

		Error(const char* message, const unsigned int line, const std::string file);
		Error(const std::string message, const unsigned int line, const std::string file);
		Error(const char* message, const unsigned int line, const char* file);
		Error(const std::string message, const unsigned int line = 0, const char* file = "No file reported");
		Error() noexcept = default;
		~Error() noexcept = default;

		void handle[[noreturn]]();

		unsigned int getLine() const;
		const char* getFile() const;
	private:
		const unsigned int line;
		const char* file;
	};

#define MACE__GET_ERROR_NAME(name) name##Error
#define MACE__DECLARE_ERROR(name) class MACE__GET_ERROR_NAME(name) : public Error{public: using Error::Error;}
#define MACE__THROW(name, message) throw MACE__GET_ERROR_NAME(name) ( std::string(__func__) + ": " + std::string(message), __LINE__, __FILE__)

	/**
	Thrown when an error from an unknown source occured
	*/
	MACE__DECLARE_ERROR(Unknown);

	/**
	Thrown when a pointer is equal to NULL
	*/
	MACE__DECLARE_ERROR(NullPointer);

	/**
	Thrown when an assertion fails.
	@see MACE::assertModule(std::string)
	*/
	MACE__DECLARE_ERROR(AssertionFailed);

	/**
	Thrown when a resource already exists and you are attempting to recreate it
	*/
	MACE__DECLARE_ERROR(AlreadyExists);

	/**
	Thrown when an object wasn't initializaed or initializtion failed
	*/
	MACE__DECLARE_ERROR(InitializationFailed);

	/**
	Thrown when an error occured trying to read or write a file
	@see FileNotFoundError
	*/
	MACE__DECLARE_ERROR(BadFile);

	/**
	Thrown when a file was not found on the filesystem
	@see BadFileError
	*/
	MACE__DECLARE_ERROR(FileNotFound);

	/**
	Thrown when a function looks for an object, but doesn't find it.
	*/
	MACE__DECLARE_ERROR(ObjectNotFound);

	/**
	Thrown when something is of the wrong type
	*/
	MACE__DECLARE_ERROR(InvalidType);

	/**
	Thrown when an index is provided for an array, but it is outside the valid bounds of the array.
	*/
	MACE__DECLARE_ERROR(IndexOutOfBounds);

	/**
	Thrown when something was in the wrong state at the time of an operation
	*/
	MACE__DECLARE_ERROR(InvalidState);

	/**
	Thrown when the process has no remaining memory available
	*/
	MACE__DECLARE_ERROR(OutOfMemory);

	/**
	Thrown when the operating system throws an error
	*/
	MACE__DECLARE_ERROR(System);

	class MultipleErrors: public Error {
	public:
		using Error::Error;

		MultipleErrors(const Error errs[], const unsigned int errorSize, const unsigned int line, const char* file);

		const char* what() const override;
	private:
		std::string message;
	};
}

#endif