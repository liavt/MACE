#pragma once
#include <exception>

namespace mc {
	struct DependencyNotFound : public std::runtime_error {
		explicit DependencyNotFound(const char* c="No message was given") : runtime_error(c) {};
		explicit DependencyNotFound(const std::string c = "No message was given") : runtime_error(c) {};
	};	
	struct ObjectNotFoundInArray : public std::runtime_error {
		explicit ObjectNotFoundInArray(const char* c = "No message was given") : runtime_error(c) {};
		explicit ObjectNotFoundInArray(const std::string c = "No message was given") : runtime_error(c) {};
	};
	struct IndexOutOfBounds : public std::runtime_error {
		explicit IndexOutOfBounds(const char* c = "No message was given") : runtime_error(c) {};
		explicit IndexOutOfBounds(const std::string c = "No message was given") : runtime_error(c) {};
	};
}