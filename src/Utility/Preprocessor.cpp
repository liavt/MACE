#include <MACE/System/Utility/Preprocessor.h>
#include <MACE/System/Constants.h>
#include <iostream>

namespace mc {

	//todo:
	/*
	digraphs (see 6.4.6 of standard)
	force preprocessor to be first letter in new line

	stringification


	#define (and cant use reserved keywords, seee 6.4.1 of standard)
	#undef

	#ifdef
	#if
	#ifndef
	#else
	#elif
	#endif

	#error
	#warning

	#include (and 15 nesting levels)

	#line

	#pragma (if STDC follows, do no macro replacement)


	MACROS (can't be undef:)
	__FILE__
	__LINE__
	__DATE__
	__STDC__
	__STDC_HOSTED__
	__STDC_VERSION__
	__MACE__
	*/

	Preprocessor Preprocessor::globalPreprocessor = Preprocessor();

	std::string Preprocessor::preprocess(const std::string & input)
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
				continue;
			}
			//if its not a backslash-newline, we check if its just a newline, obviously multiline comments ignore newlines
			else if ((value == '\n')&&state!=MULTILINE_COMMENT) { 
				state = NEWLINE;
			}
			
			//now we start the switch_case of the state machine.
			switch (state) {
			default:
				state = PROBING;

			case PROBING:
				if (value == '#')state = FINDING_COMMAND_START;
				else if (value == '\"' || value == '\'')state = STRING_LITERAL;
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
				break;

			case PARAMETERS:
				params += value;
				break;

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
				command = "";
				params = "";
				state = PROBING;
				break;
			}

			out += value;
		}

		return out;
	}

	std::string Preprocessor::executeCommand(const std::string & command, const std::string & params)
	{
		return std::string();
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

	Preprocessor & Preprocessor::getGlobalPreprocessor()
	{
		return globalPreprocessor;
	}

}