#pragma once
#include <vector>
#include <string>
#include <MACE/System/Exceptions.h>


namespace mc {

using PreprocessorException = Exception;

class Preprocessor {
public:
	Preprocessor(const std::string& input,const std::string& filename="Unknown file");

	std::string preprocess();

	void addIncludeDirectory(std::string& directory);

	std::vector<std::string> getIncludeDirectories();
	const std::vector<std::string> getIncludeDirectories() const;
	void setIncludeDirectories(const std::vector<std::string> directories);

	const std::string getInput() const;

	std::string getFilename();
	const std::string getFilename() const;
	void setFilename(const std::string& filename);

	unsigned int getLine();
	const unsigned int getLine() const;
	void setLine(const unsigned int line);

	void defineMacro(const std::string& name, const std::string& definition);
	bool isMacroDefined(const std::string& name);
	void undefineMacro(const std::string& name);

	const std::string& getMacro(const std::string& name) const;

	/**
	Generates a string with the current line number and file name.
	@return The current location in the file
	@see getLine()
	@see getFilename()
	*/
	std::string getLocation() const;

private:

	static std::vector<std::string> reservedWords;

	const std::string input;

	unsigned int line = 0;

	std::string filename = "";



	std::vector<std::string> includeDirectories = std::vector<std::string>();
	std::vector<std::pair<std::string,std::string>> macros = std::vector<std::pair<std::string,std::string>>();

	std::string parse(const std::string& input);

	std::string executeDirective(bool& outputValue,const std::string& command, const std::string& params);

	int getMacroLocation(const std::string& name);



};

}