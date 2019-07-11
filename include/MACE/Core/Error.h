/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__CORE_ERROR_H
#define MACE__CORE_ERROR_H
#include <string>
#include <stdexcept>
#include <MACE/Core/Constants.h>

#if !defined(__cpp_exceptions) || __cpp_exceptions < 199711
#	error C++ exceptions must be supported!
#endif

namespace mc {
	//forward declaration for handleError();
	class Instance;

	namespace internal {

		/**
		Superclass that all exceptions in MACE extend.
		<p>
		Do not use this class directly. Instead, use the `MACE_THROW_CUSTOM_LINE` or `MACE_THROW` macros.
		To declare a new type of `Error`, use the `MACE_DECLARE_NAME` macro.
		<p>
		Use `std::exception` for catching MACE exceptions, as they all inherit from that.
		@tparam ErrorType A `CString` representing the name of this error
		@internal
		*/
		template<typename ErrorType, typename Base>
		class Error: public Base {
		public:
#ifdef MACE_DEBUG
			Error(const std::string& message, const std::string& line, const std::string& file) noexcept : Base(std::string(ErrorType::val) + ": " + message + "\n\t[" + line + " @ " + file + "]") {}
#else
			Error(const std::string& message, const std::string&, const std::string&) noexcept : Base(message) {}
#endif//MACE_DEBUG
		};
	}

#define MACE__GET_ERROR_NAME(name) name##Error
#ifdef MACE_DEBUG
#	define MACE__DECLARE_ERROR_TYPE(name) struct name##Type { static MACE_CONSTEXPR CString val = MACE_STRINGIFY_DEFINITION(MACE__GET_ERROR_NAME(name)); }
#else
#	define MACE__DECLARE_ERROR_TYPE(name) struct name##Type {}
#endif//MACE_DEBUG
#define MACE__DECLARE_ERROR_BASE(name, base) MACE__DECLARE_ERROR_TYPE(name); using MACE__GET_ERROR_NAME(name) = MACE__INTERNAL_NS::Error< name##Type, base >;
#define MACE__DECLARE_ERROR(name) MACE__DECLARE_ERROR_BASE(name, std::runtime_error)
#define MACE__THROW_CUSTOM_LINE(name, message, line, file) do{throw ::mc::MACE__GET_ERROR_NAME(name) ( std::string(MACE_FUNCTION_NAME) +  ": " + std::string(message), line, file);}while(0)
#define MACE__THROW(name, message) MACE__THROW_CUSTOM_LINE(name, message, MACE_STRINGIFY_DEFINITION(__LINE__), __FILE__)

#ifndef MACE_ASSERT
#	ifdef MACE_DEBUG
#		define MACE_ASSERT(cond, message) do{if(!( cond )){MACE__THROW(AssertionFailed, std::string("Assertion failed: " MACE_STRINGIFY(cond) ": ") + message);}}while(0)
#	else
#		define MACE_ASSERT(cond, message)
#	endif//MACE_DEBUG
#endif//MACE_ASSERT

	/**
	Thrown when an error from an unknown source occured
	*/
	MACE__DECLARE_ERROR(Unknown);

	/**
	Thrown when a pointer is equal to `nullptr`
	*/
	MACE__DECLARE_ERROR_BASE(NullPointer, std::logic_error);

	/**
	Thrown when an assertion fails.
	@see MACE::assertModule(std::string)
	*/
	MACE__DECLARE_ERROR_BASE(AssertionFailed, std::logic_error);

	/**
	Thrown when a resource already exists and you are attempting to recreate it
	@see Instance
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
	MACE__DECLARE_ERROR_BASE(OutOfBounds, std::out_of_range);

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

	std::string getErrorDump(const std::exception& e, const Instance* i = nullptr);

	/**
	Stops MACE and prints an exception to console accordingly. This should be used every time a fatal exception is thrown.
	*/
	void handleError MACE_NORETURN(const std::exception& e, Instance* i = nullptr);// i is not const because requestStop() will be called

	/**
	@copydoc Error::handleError(const std::exception& e, Instance*)
	*/
	void handleError MACE_NORETURN(const std::exception& e, Instance& i);
}

#endif