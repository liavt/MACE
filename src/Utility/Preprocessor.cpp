#include <MACE/System/Utility/Preprocessor.h>
#include <MACE/System/Constants.h>
#include <iostream>

namespace mc {

	//todo:
	/*
	#define
	#undef

	#ifdef
	#if
	#ifndef
	#else
	#elif
	#endif

	#error
	#warning

	#include

	#line

	#pragma


	MACROS:
	__FILE__
	__LINE__
	__DATE__
	__STDC__
	__STDC_VERSION__
	__MACE__
	*/

	std::vector<std::string> Preprocessor::includeDirectories = std::vector<std::string>();

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
			//if its not a backslash-newline, we check if its just a nexline
			else if ((value == '\n')) { 
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
				//prevent it from outputing the comment
				value = '\0';
				break;

			case MULTILINE_COMMENT:
				//input.length() is minus 1 because we check the next value, so just in case the input ends with the * character
				for (iter;iter<input.length()-1;++iter) {
					//do nothing until */
					if (value == '*'&&input[value + 1] == '/') {
						//we increment iter to account for the next / character
						++iter;
						break;
					}
				}
				break;

			case NEWLINE:
				command = "";
				params = "";
				state = PROBING;
				break;
			}

			//the only reason there would be a \0 is because we added one to the value, as the std::string.length() is the actual length of the string
			if(value!='\0')out += value;
		}

		return out;
	}

	void Preprocessor::addIncludeDirectory(std::string & directory)
	{
		includeDirectories.push_back(directory);
	}

}