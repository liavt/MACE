#pragma once
#include <vector>
#include <string>
#include <map>
#include <MACE/System/Exceptions.h>


namespace mc {

/**
`Exception` thrown when a syntax error is encountered, or #error is called.	
*/
using PreprocessorException = Exception;



class Preprocessor {
public:
	struct Macro {
		Macro(std::string name, std::string definition, std::vector < std::string > parameters,std::string parameterString);
		Macro(std::string name,std::string definition);

		std::string name;
		std::string definition;
		std::vector < std::string > parameters;
		std::string parameterString;
	};


	Preprocessor(const std::string& input,const std::string& filename="Unknown file");
	explicit Preprocessor(const std::string& input, const Preprocessor& clone);

	std::vector< std::string > preprocessTokens();
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

	void defineMacro(const Macro& macro);
	bool isMacroDefined(const std::string& name) const;
	void undefineMacro(const std::string& name);

	const Macro& getMacro(const std::string& name) const;

	/**
	Generates a string with the current line number and file name.
	@return The current location in the file
	@see getLine()
	@see getFilename()
	*/
	std::string getLocation() const;

private:

	static const std::string reservedWords[];

	//i dont have a pun about pun-ctuators

	//punctuators constitute new tokens
	//punctuators with 1 character (like >)
	static const std::vector< char > punctuators1c;
	//punctuators with 2 characters (like >>)
	static const std::vector< std::string > punctuators2c;


	const std::string input;

	unsigned int line = 0;

	std::string filename = "";



	std::vector< std::string > includeDirectories;
	std::vector< Macro > macros;

	std::vector< std::string > parse(const std::string& input);

	std::vector< std::string > executeDirective(bool& outputValue,const std::string& command, const std::string& params);

	int getMacroLocation(const std::string& name) const;

	bool isNewToken(const char lastValue, const char value) const;
	bool isNonOperator(const char value) const;

	std::string expandMacro(const std::string input) const;

	int getIfScopeLocation();
	int getCurrentIfScopeLocation();

	void calculateIfScope(bool& outputValue, const bool statementPassed);
	bool parseIfStatement(const std::string statement);

	Macro parseMacroName(const std::string& name) const;

	/**
	This function doesnt check for reserveed keywords, as opposed to defineMacro which does. defineMacro also uses this function
	*/
	void setMacro(const Macro& m);

};

}