#include <MACE/System/Utility/Preprocessor.h>
#include <MACE/System/Constants.h>
#include <iostream>

namespace mc {

	//todo:
	/*
	digraphs (see 6.4.6 of standard)

	macro expansion

	functional macros and __VA_ARGS__

	self-referntial macros up to 1 recurse

	stringification

	#ifdef
	#if, !, &&, (), ||
	#ifndef
	#else
	#elif
	#endif

	#if defined () or # if defined name

	#include (and 15 nesting levels)

	#pragma (if STDC follows, do no macro replacement)


	MACROS (can't be undef:)
	__FILE__
	__LINE__
	__DATE__
	__STDC__
	__STDC_HOSTED__
	__STDC_VERSION__
	__MACE__
	__INCLUDE_LEVEL__
	__MACRO_NUMBER__
	*/

	std::vector<std::string> Preprocessor::reservedWords = {

		"and", "and_eq", "asm",	"auto",	"bitand",
		"bitor", "bool",	"break",	"case",	"catch",
		"char", "class",	"const",	"const_cast",	"continue",
		"default","delete",	"do",	"double",	"dynamic_cast",
		"else", "enum",	"explicit",	"export",	"extern",
		"false", "float",	"for",	"friend",	"goto",
		"if", "inline",	"int",	"long",	"mutable",
		"namespace", "new",	"not", "not_eq", "operator",
		"or", "or_eq", "private",	"protected",	"public",
		"register", "reinterpret_cast",	"return",	"short",	"signed",
		"sizeof", "static",	"static_cast",	"struct",	"switch",
		"template", "this",	"throw", "true", "try",
		"typedef", "typeid",	"typename", "union",	"unsigned",
		"using", "virtual",	"void", "volatile", "wchar_t",
		"while",	"xor", "xor_eq"
	
	};

	Preprocessor::Preprocessor(const std::string& inputString, const std::string& filename) : input(inputString)
	{
		this->filename = filename;
	}

	std::string Preprocessor::preprocess()
	{/**/
		return parse(input);
	}

	std::string Preprocessor::parse(const std::string & input)
	{
		std::string command = "", params = "", out = "";

		/*
		We use a state machine to preprocess. Whenever we #include a file, we also recursively run preprocess on it.

		PROBING starts first. If it finds a # character, it goes to FINDING_COMMAND_START. If it finds a " or ', it goes to STRING_LITERAL. If it finds a //, it goes to SINGLELINE_COMMENT

		FINDING_COMMAND_START looks for the first non-whitespace character, and when it does, goes to COMMAND_NAME
		COMMAND_NAME adds every character to command until it encounters a whitespace character, where it goes to PARAMETERS
		PARAMETERS adds every character to params

		STRING_LITERAL does nothing until it finds " or ', where it goes back to PROBING

		SINGELINE_COMMENT does nothing and doesn't add to the output until the next newline
		MULTILINE_COMMENT does nothing and doesn't add to the output until *\/

		If the next character is a newline, it immediately goes into the NEWLINE state

		NEWLINE processes command and params if they exist, and then sends state back to PROBING_POUND_SIGN
		*/
		enum STATE { PROBING, NEWLINE, FINDING_COMMAND_START, COMMAND_NAME, PARAMETERS, STRING_LITERAL, SINGLELINE_COMMENT,MULTILINE_COMMENT};

		enum STATE state = PROBING;

		for (Index iter = 0; iter < input.length(); ++iter) {
			char value = input[iter];
			
			//this if statement does the backslash-newline, and converts it to a space by checking the current character is \ and the next one is \n
			if (value=='\\'&&(iter < input.length()-1 && input[iter + 1] == '\n')) {
				++iter;
				++line;
				continue;
			}
			//if its not a backslash-newline, we check if its just a newline, obviously multiline comments ignore newlines
			else if ((value == '\n')) {
				if (state != MULTILINE_COMMENT){
					state = NEWLINE;
				}
				++line;
			}
			
			//now we start the switch_case of the state machine.
			switch (state) {
			default:
				state = PROBING;

			case PROBING:
				//we check if the character is #, and if its the first character in the line. Also checks if its the first line, then dont check below zero
				if (value == '#') {
					if(((iter>0 && input[iter - 1] == '\n') || iter == 0)){
						state = FINDING_COMMAND_START;
						continue;
					}
				}
				else if (value == '\"' || value == '\''){
					state = STRING_LITERAL;
					break;
				}
				else if (value == '/') {
					if (iter < input.length() - 1) {
						char nextValue = input[iter + 1];
						if (nextValue == '/'){
							state = SINGLELINE_COMMENT;

							++iter;
							continue;
						}else if (nextValue == '*') {
							state = MULTILINE_COMMENT;

							++iter;
							continue;
						}
					}
				}
				break;

			case FINDING_COMMAND_START:
				if (value != '\t' || value != ' ')state = COMMAND_NAME;

			case COMMAND_NAME:
				if (value == ' ') {
					state = PARAMETERS;
				}
				else {
					command += value;
				}
				continue;

			case PARAMETERS:
				params += value;
				continue;

			case STRING_LITERAL:
				if (value == '\"' || value == '\'')state = PROBING;
				break;

			case SINGLELINE_COMMENT:
				continue;

			case MULTILINE_COMMENT:
				if (value == '/'&&input[iter - 1] == '*') {
					//we increment iter to account for the next / character
					state = PROBING;
				}
				continue;
		
			case NEWLINE:
				if (command != "") {
					out += executeDirective(command,params);
				}
				command = "";
				params = "";
				state = PROBING;
				break;
			}

			out += value;
		}

		return out;
	}

	std::string Preprocessor::executeDirective(const std::string & command, const std::string & params)
	{
		if (command == "error") {
			if (params == "")throw PreprocessorException(getLocation() + ": #error must be called with a message");
			throw PreprocessorException(getLocation() + ": "+params);
		}
		else if (command == "warning") {
			if (params == "")throw PreprocessorException(getLocation() + ": #warning must be called with a message");
			std::cout << getLocation() <<": "<<params<<std::endl;
		}
		else if (command == "line") {
			if (params == "")throw PreprocessorException(getLocation() + ": #line must be called with a line number");
			
			Index iterator;
			std::string newLineNumber = "";
			std::string newFileName = "";

			//first we get the line number
			for (iterator = 0; iterator < params.length(); ++iterator) {
				if (params[iterator] == ' ') {
					++iterator;//lets also increment the iterator so the space doesnt get added to the filename
					break;//the second word, lets go to the second loop
				}
				newLineNumber += params[iterator];
			}

			//remaining characters are added to the filename
			for (iterator; iterator < params.length(); ++iterator) {
				newFileName += params[iterator];
			}

			//the filename is optional
			if (newFileName != "") {
				setFilename(newFileName);
			}

			setLine(std::stoi(newLineNumber));
		}
		else if (command == "define") {
			if (params == "")throw PreprocessorException(getLocation() + ": #define must be called with a name and definition");

			Index iterator;
			std::string macroName = "";
			std::string macroDefinition = "";

			//first we get the line number
			for (iterator = 0; iterator < params.length(); ++iterator) {
				if (params[iterator] == ' ') {
					++iterator;//lets also increment the iterator so the space doesnt get added to the filename
					break;//the second word, lets go to the second loop
				}
				macroName += params[iterator];
			}

			//remaining characters are added to the filename
			for (iterator; iterator < params.length(); ++iterator) {
				macroDefinition += params[iterator];
			}

			if(macroDefinition=="")throw PreprocessorException(getLocation() + ": a macro must have a definition");
			
			defineMacro(macroName,macroDefinition);
		}
		else if (command == "undef") {
			if (params == "")throw PreprocessorException(getLocation() + ": #undef must be called with a macro name");

			Index iterator;
			std::string macroName = "";

			//first we get the line number
			for (iterator = 0; iterator < params.length(); ++iterator) {
				if (params[iterator] == ' ') {
					++iterator;//lets also increment the iterator so the space doesnt get added to the filename
					break;//the second word, lets go to the second loop
				}
				macroName += params[iterator];
			}

			undefineMacro(params);
		}
		else {
			throw PreprocessorException(getLocation()+": Unknown preprocessing directive: "+command);
		}

		return "";
	}

	void Preprocessor::defineMacro(const std::string & name, const std::string & definition)
	{
		unsigned int iterator = 0;
		for (iterator = 0; iterator < reservedWords.size(); ++iterator) {
			if (reservedWords[iterator] == name)throw PreprocessorException(getLocation()+": can\'t define a reserved word");
		}
		for (iterator = 0; iterator < macros.size(); ++iterator) {
			if (macros[iterator].first == name) {
				macros[iterator].second = definition;
			}
		}
		macros.push_back(std::make_pair(name,definition));
	}

	void Preprocessor::undefineMacro(const std::string & name)
	{
		unsigned int iterator = 0;
		for (iterator = 0; iterator < reservedWords.size(); ++iterator) {
			if (reservedWords[iterator] == name)throw PreprocessorException(getLocation() + ": can\'t undefine a reserved word");
		}
		for (iterator = 0; iterator < macros.size(); ++iterator) {
			if (macros[iterator].first == name) {
				macros.erase(macros.begin()+iterator);
				return;
			}
		}

		//do nothing if its not found
	}

	const std::string & Preprocessor::getMacro(const std::string & name) const
	{
		for (unsigned int iterator = 0; iterator < macros.size(); ++iterator) {
			if (macros[iterator].first == name) {
				return macros[iterator].second;
			}
		}
		throw PreprocessorException(getLocation() + ": Unknown macro "+name);
	}

	std::string Preprocessor::getLocation() const
	{
		return "Line " +std::to_string(line)+" in "+(filename);
	}

	void Preprocessor::addIncludeDirectory(std::string & directory)
	{
		includeDirectories.push_back(directory);
	}

	std::vector<std::string> Preprocessor::getIncludeDirectories()
	{
		return includeDirectories;
	}

	const std::vector<std::string> Preprocessor::getIncludeDirectories() const
	{
		return includeDirectories;
	}

	void Preprocessor::setIncludeDirectories(const std::vector<std::string> directories)
	{
		this->includeDirectories = directories;
	}

	const std::string Preprocessor::getInput() const
	{
		return input;
	}

	std::string Preprocessor::getFilename()
	{
		return filename;
	}

	const std::string Preprocessor::getFilename() const
	{
		return filename;
	}

	void Preprocessor::setFilename(const std::string & filename)
	{
		this->filename = filename;
	}

	unsigned int Preprocessor::getLine()
	{
		return line;
	}

	const unsigned int Preprocessor::getLine() const
	{
		return line;
	}

	void Preprocessor::setLine(const unsigned int line)
	{
		this->line = line;
	}

}