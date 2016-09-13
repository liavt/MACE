#pragma once
#include <vector>
#include <string>
#include <MACE/System/Exceptions.h>

#define INCLUDES_X_MACRO \
	INCLUDE_FUNCTION(define) \
	INCLUDE_FUNCTION(undef) \
	INCLUDE_FUNCTION(ifdef) \
	INCLUDE_FUNCTION(if) \
	INCLUDE_FUNCTION(ifndef) \
	INCLUDE_FUNCTION(else) \
	INCLUDE_FUNCTION(elif) \
	INCLUDE_FUNCTION(endif) \
	INCLUDE_FUNCTION(error) \
	INCLUDE_FUNCTION(warning) \
	INCLUDE_FUNCTION(include) \
	INCLUDE_FUNCTION(line) \
	INCLUDE_FUNCTION(pragma)


namespace mc {

using PreprocessorException = Exception;

class Preprocessor {
public:


	std::string preprocess(const std::string& input);

	void addIncludeDirectory(std::string& directory);

	std::vector<std::string> getIncludeDirectories();
	const std::vector<std::string> getIncludeDirectories() const;

	static Preprocessor& getGlobalPreprocessor();
private:

	static Preprocessor globalPreprocessor;

	std::vector<std::string> includeDirectories = std::vector<std::string>();

	std::string parse(const std::string& input);

	std::string executeCommand(const std::string& command, const std::string& params);


};

}