#pragma once
#include <exception>

namespace mc {
	struct DependencyNotFound : public std::runtime_error {
		explicit DependencyNotFound(const char* c="No message was given") : runtime_error(c) {};
		explicit DependencyNotFound(const std::string c = "No message was given") : runtime_error(c) {};
	};
}