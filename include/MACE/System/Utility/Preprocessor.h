#pragma once
#include <vector>
#include <string>
#include <MACE/System/Exceptions.h>

namespace mc {

using PreprocessorException = Exception;

class Preprocessor {
public:
	static std::string preprocess(const std::string& input);

	static void addIncludeDirectory(std::string& directory);
private:

	static std::string parse(const std::string& input);

	static std::vector<std::string> includeDirectories;
};

}