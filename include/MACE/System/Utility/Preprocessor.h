/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#include <vector>
#include <string>
#include <map>
#include <MACE/System/Exceptions.h>


namespace mc {

/**
`Exception` thrown when a syntax error is encountered, or \#error is called.	
*/
using PreprocessorException = Exception;

struct Macro {
	Macro(std::string name, std::string definition, std::vector < std::string > parameters, std::string parameterString);
	Macro(std::string name, std::string definition);

	std::string name;
	std::string definition;
	std::vector < std::string > parameters;
	std::string parameterString;
};

class Include {
public:
	virtual bool hasFile(const std::string& name) const = 0;
	virtual std::string getFile(const std::string& name) const = 0;
};

class IncludeString : public Include {
public:
	IncludeString(const std::string& content, const std::string name);

	bool hasFile(const std::string& name) const;
	std::string getFile(const std::string& name) const;

private:

	const std::string content = "";
	const std::string name = "";
};

class IncludeDirectory : public Include {
public:
	IncludeDirectory(const std::string& dir);

	bool hasFile(const std::string& name) const;
	std::string getFile(const std::string& name) const;

private:

	const std::string directory = "";
};

class Preprocessor {
public:



	Preprocessor(const std::string& input,const std::string& filename="Unknown file");
	explicit Preprocessor(const std::string& input, const Preprocessor& clone);

	std::vector< std::string > preprocessTokens();
	std::string preprocess();

	void addInclude(Include& incl);

	std::vector< Include* > getIncludes();
	const std::vector< Include* > getIncludes() const;
	void setIncludes(const std::vector< Include* > directories);

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

	void defineStandardMacros();
	void defineOSMacros();

	const Macro& getMacro(const std::string& name) const;

	/**
	Generates a string with the current line number and file name.
	@return The current location in the file
	@see getLine()
	@see getFilename()
	*/
	std::string getLocation() const;

private:
	const std::string input;

	unsigned int line = 0;

	std::string filename = "";

	std::vector< Include* > includes;
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
	This function doesnt check for reserved keywords, as opposed to defineMacro which does. defineMacro also uses this function
	*/
	void setMacro(const Macro& m);

};

}