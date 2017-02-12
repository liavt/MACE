/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Utility/Preprocessor.h>

#include <MACE/Core/Constants.h>
#include <MACE/Core/System.h>

#include <iostream>
#include <cctype>
#include <ctime>
#include <cstring>
#include <algorithm>
#include <memory>
#include <fstream>
#include <array>

//indirection is the only way to expand macros in other macros
#define MACE_NAME_STRINGIFY(name) "" #name
#define MACE_STRINGIFY(name) #name
//the strcmp checks if the macro is defined. if the name is different from it expanded, then it is a macro. doesnt work if a macro is defined as itself, but that shouldnt happen
#define MACE_PREDEFINE_MACRO(name) if(std::strcmp("" #name ,MACE_NAME_STRINGIFY(name))){setMacro( Macro( #name , MACE_STRINGIFY( name ) ));}

namespace mc {

	namespace {
		//these words can't be defined or undefined
		const std::string reservedWords[] = {
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
			"while",	"xor", "xor_eq","defined","__FILE__","__LINE__",
			"__TIME__","__DATE__","__STDC__","__STDC_HOSTED__","__STDC_VERSION__",
			"__MACE__","__IF_SCOPE__","__CURRENT_IF_SCOPE__","__VA_ARGS__"
		};

		//a punctuator according to the standard is something that seperates tokens.
		/**
		1 character long punctuators. The reason that 2 character long and 1 characer long punctuators are seperate arrays
		is because something like == has 1 punctuator, but !! has 2 punctuators.
		*/
		const std::array< char, 23 > punctuators1c = {
			',','\"','\'','{','}','[',']','~','.','|','&','+','-','*','/','=',';','!','%','>','<',':','?'
		};

		/**
		2 character long punctuators
		@todo Make this constexpr. Right now, it is allocated at runtime, not compile time. std::string is not constexpr so an alternative is needed
		*/
		const std::array< std::string, 20 > punctuators2c = {
			">>","<<","++","--","+=","-=","*=","/=","&=","|=","%=","==","!=",">=","<=","&&","||","->","::","##"
		};

		bool isNonOperator(const char value) {
			return std::find(punctuators1c.begin(), punctuators1c.end(), value) == punctuators1c.end();
		}

		bool isNewToken(const char lastValue, const char value) {

			//This is quite a complex if statement, but it checks to make sure that == doesnt become =,= and a=b becomes a,=,b
			return (std::isspace(value)
					|| ((std::find(punctuators1c.begin(), punctuators1c.end(), lastValue) != punctuators1c.end()
					|| (!isNonOperator(lastValue) && isNonOperator(value))
					|| (isNonOperator(lastValue) && !isNonOperator(value))
					)
					&& (std::find(punctuators2c.begin(), punctuators2c.end(), std::string{ lastValue,value }) == punctuators2c.end())
					));
		}
	}

	Preprocessor::Preprocessor(const std::string& inputString, const std::string& filename) : input(inputString) {
		this->filename = filename;
	}

	Preprocessor::Preprocessor(const std::string & input, const Preprocessor & clone) : Preprocessor(input, clone.getFilename()) {
		includes = clone.includes;
		macros = clone.macros;
		line = clone.line;
	}

	std::vector<std::string> Preprocessor::preprocessTokens() {
		setMacro(Macro("__MACE__", "1"));

		if( getMacroLocation("__INCLUDE_LEVEL__") < 0 ) {
			setMacro(Macro("__INCLUDE_LEVEL__", "0"));
		}

		return parse();
	}

	std::string Preprocessor::preprocess() {


		std::string out = "";
		std::vector < std::string > tokens = preprocessTokens();

		for( unsigned int i = 0; i < tokens.size(); i++ ) {
			out += tokens[i];
		}
		return out;
	}

	std::vector < std::string > Preprocessor::parse() {
		std::string command = "", params = "", currentToken = "";

		std::vector < std::string > tokens = std::vector < std::string >();

		/*
		We use a state machine to preprocess. Whenever we #include a file, we also recursively run preprocess on it.

		PROBING starts first. If it finds a # character, it goes to FINDING_COMMAND_START. If it finds a " or ', it goes to STRING_LITERAL. If it finds a //, it goes to SINGLELINE_COMMENT. If it finds a ( that has a letter before it, it goes to FUNCTION

		FINDING_COMMAND_START looks for the first non-whitespace character, and when it does, goes to COMMAND_NAME
		COMMAND_NAME adds every character to command until it encounters a whitespace character, where it goes to PARAMETERS
		PARAMETERS adds every character to params

		STRING_LITERAL does nothing until it finds " or ', where it goes back to PROBING

		SINGELINE_COMMENT does nothing and doesn't add to the output until the next newline
		MULTILINE_COMMENT does nothing and doesn't add to the output until *\/

		FUNCTION does not create a new token until a ')' character is encountered, where it goes back to PROBING.

		If the next character is a newline, it immediately goes into the NEWLINE state

		NEWLINE processes command and params if they exist, and then sends state back to PROBING_POUND_SIGN
		*/
		enum: Byte {
			PROBING, NEWLINE, FINDING_COMMAND_START, COMMAND_NAME, PARAMETERS,
			STRING_LITERAL, SINGLELINE_COMMENT, MULTILINE_COMMENT, PARENTHESIS
		} state = PROBING;

		/*
		If set to true, the current character will be added to final output. This is used when an #if/#ifdef/#ifndef/#elif is called with a false condition.
		This is used because a false if condition will last multiple lines. The only other thing that doesnt output for multiple lines is a multiline comment, however it only needs to check for multiple characters.

		Instead of using outputvalue, continue should be used instead.
		*/
		bool outputValue = true;

		//used to accurately parse function(function(arg))
		int parenScope = 0;

		for( Index iter = 0; iter <= input.length(); ++iter ) {
			char value;
			//we add a newline at the end, because at the end of a line we do special calculations, and one liners need to do them as well
			if( iter == input.length() ) {
				value = '\n';
			} else {
				value = (input[iter]);
			}

			//this if statement does the backslash-newline, and converts it to a space by checking the current character is \ and the next one is \n
			if( value == '\\' && (iter < input.length() - 1 && input[iter + 1] == '\n') ) {
				++iter;
				++line;
				setMacro(Macro("__LINE__", std::to_string(line)));
				continue;
			}
			//if its not a backslash-newline, we check if its just a newline, obviously multiline comments ignore newlines
			else if( value == '\n' ) {
				//if its in the middle of parenthesis, continue on
				if( state != MULTILINE_COMMENT&&state != PARENTHESIS ) {
					state = NEWLINE;
				}
				++line;
				setMacro(Macro("__LINE__", std::to_string(line)));
			}

			//now we start the switch_case of the state machine.
			switch( state ) {
			default:
				state = PROBING;

			case PROBING:
				//we check if the character is #, and if its the first character in the line. Also checks if its the first line, then dont check below zero
				if( value == '#' ) {
					if( ((iter > 0 && (std::iscntrl(input[iter - 1]))) || iter == 0) ) {
						state = FINDING_COMMAND_START;
						continue;
					}
				} else if( value == '\"' || value == '\'' ) {
					state = STRING_LITERAL;
					break;
				} else if( value == '/' ) {
					if( iter < input.length() - 1 ) {
						char nextValue = input[iter + 1];
						if( nextValue == '/' ) {
							state = SINGLELINE_COMMENT;

							++iter;
							continue;
						} else if( nextValue == '*' ) {
							state = MULTILINE_COMMENT;

							++iter;
							continue;
						}
					}
				} else if( value == '\0' ) {
					value = ' ';
				} else if( value == '(' ) {
					++parenScope;
					state = PARENTHESIS;
				}

				break;

			case FINDING_COMMAND_START:
				if( !std::isspace(value) ) {
					state = COMMAND_NAME;
					//why are we adding it here? if we directly go to COMMAND_NAME, it will notice the previous character is space and think its time for parameters
					command += value;
					continue;
				}


			case COMMAND_NAME:
				//we check if its not a space
				if( !std::isspace(value) ) {
					//i can do iter-1 because at this point, a # and a command name must have been called, so no negative indices
					//if the previous character was a space, then obviously this is the beginning of the parameters
					if( std::isspace(input[iter - 1]) ) {
						state = PARAMETERS;
					} else {
						command += value;
						continue;
					}
				} else {
					continue;
				}

			case PARAMETERS:
				params += value;
				continue;

			case STRING_LITERAL:

				if( value == '"' || value == '\'' ) {
					state = PROBING;
				}

				if( outputValue )currentToken += value;
				continue;

			case SINGLELINE_COMMENT:
				continue;

			case MULTILINE_COMMENT:
				if( value == '/'&&input[iter - 1] == '*' ) {
					//we increment iter to account for the next / character
					state = PROBING;
				}
				continue;

			case PARENTHESIS:
				if( outputValue )currentToken += value;
				if( value == ')' ) {
					--parenScope;
					if( parenScope == 0 ) {
						state = PROBING;
					}
				} else if( value == '(' ) {
					++parenScope;
				}
				continue;

			case NEWLINE:
				if( !command.empty() ) {
					//outputValue is passed by reference so it can be changed
					std::vector < std::string > directiveTokens = executeDirective(outputValue, command, params);

					tokens.insert(tokens.end(), directiveTokens.begin(), directiveTokens.end());
				}
				command = "";
				params = "";
				state = PROBING;
				break;
			}

			if( outputValue ) {
				if( !currentToken.empty() && (isNewToken(currentToken.back(), value)) ) {
					tokens.push_back(expandMacro(currentToken));
					currentToken = "";
				}

				currentToken += value;
			}
		}


		//I had this at one point, but now it doesnt work. It seems necessary, but I don't know why it doesnt work.
		/*
		const int macroLocation = getMacroLocation("__IF_SCOPE__");
		if( macroLocation >= 0 ) {
			if( std::stoi(getMacro("__IF_SCOPE__").definition) > 0 ) {
				throw PreprocessorError(getLocation() + ": #if directive is missing an #endif");
			}
		}
		*/

		return tokens;
	}

	std::vector < std::string > Preprocessor::executeDirective(bool& outputValue, const std::string & command, const std::string & params) {
		//only if we output
		if( command == "if" ) {
			if( params.empty() ) {
				throw PreprocessorError(getLocation() + ": #if must be called with an expression");
			}

			calculateIfScope(outputValue, parseIfStatement(params));

			return std::vector< std::string >();
		} else if( command == "elif" ) {
			if( params.empty() ) {
				throw PreprocessorError(getLocation() + ": #elif must be called with an expression");
			}

			calculateIfScope(outputValue, parseIfStatement(params));

			return std::vector< std::string >();
		} else if( command == "ifdef" ) {
			if( params.empty() ) {
				throw PreprocessorError(getLocation() + ": #ifdef must be called with a macro name");
			}

			Index iterator;
			std::string macroName = "";

			//first we get the line number
			for( iterator = 0; iterator < params.length(); ++iterator ) {
				if( std::isspace(params[iterator]) ) {
					++iterator;//lets also increment the iterator so the space doesnt get added to the filename
					break;//the second word, lets go to the second loop
				}
				macroName += params[iterator];
			}

			calculateIfScope(outputValue, isMacroDefined(macroName));

			return std::vector< std::string >();
		} else if( command == "ifndef" ) {
			if( params.empty() ) {
				throw PreprocessorError(getLocation() + ": #ifdef must be called with a macro name");
			}

			Index iterator;
			std::string macroName = "";

			//first we get the line number
			for( iterator = 0; iterator < params.length(); ++iterator ) {
				if( std::isspace(params[iterator]) ) {
					++iterator;//lets also increment the iterator so the space doesnt get added to the filename
					break;//the second word, lets go to the second loop
				}
				macroName += params[iterator];
			}

			calculateIfScope(outputValue, !isMacroDefined(macroName));

			return std::vector< std::string >();
		} else if( command == "else" ) {
			int ifScope = getMacroLocation("__IF_SCOPE__");
			if( ifScope < 0 || std::stoi(macros[ifScope].definition) <= 0 ) {
				throw PreprocessorError(getLocation() + ": #else is missing an if directive");
			}

			int currentIfScope = getMacroLocation("__CURRENT_IF_SCOPE__");
			if( currentIfScope < 0 ) {
				throw PreprocessorError(getLocation() + ": #else is missing an if directive");
			}


			if( outputValue || macros[ifScope].definition == macros[currentIfScope].definition ) {
				outputValue = !outputValue;
				if( outputValue ) {
					macros[currentIfScope].definition = "0";
				} else {
					macros[currentIfScope].definition = macros[ifScope].definition;
				}
			}

			return std::vector< std::string >();
		} else if( command == "endif" ) {
			int ifScope = getMacroLocation("__IF_SCOPE__");
			if( ifScope < 0 || std::stoi(macros[ifScope].definition) <= 0 ) {
				throw PreprocessorError(getLocation() + ": #endif is missing an if directive");
			}

			int currentIfScope = getMacroLocation("__CURRENT_IF_SCOPE__");
			if( currentIfScope < 0 ) {
				throw PreprocessorError(getLocation() + ": #endif is missing an if directive");
			}


			if( !outputValue&&macros[ifScope].definition == macros[currentIfScope].definition ) {
				outputValue = true;
				setMacro(Macro("__CURRENT_IF_SCOPE__", "0"));
			}

			macros[ifScope].definition = std::to_string(std::stoi(macros[ifScope].definition) - 1);

			return std::vector< std::string >();
		} else if( outputValue ) {
			if( command == "error" ) {
				if( params.empty() ) {
					throw PreprocessorError(getLocation() + ": #error was called with no message");
				}

				throw PreprocessorError(getLocation() + ": " + params);
			} else if( command == "warning" ) {
				if( params.empty() ) {
					throw PreprocessorError(getLocation() + ": #warning must be called with a message");
				}

				std::cout << getLocation() << ": " << params << std::endl;

				return std::vector< std::string >();
			} else if( command == "line" ) {
				if( params.empty() ) {
					throw PreprocessorError(getLocation() + ": #line must be called with a line number");
				}

				Index iterator;
				std::string newLineNumber = "";
				std::string newFileName = "";

				//first we get the line number
				for( iterator = 0; iterator < params.length(); ++iterator ) {
					if( params[iterator] == ' ' ) {
						++iterator;//lets also increment the iterator so the space doesnt get added to the filename
						break;//the second word, lets go to the second loop
					}
					newLineNumber += params[iterator];
				}

				//remaining characters are added to the filename
				for( ; iterator < params.length(); ++iterator ) {
					newFileName += params[iterator];
				}

				//the filename is optional
				if( newFileName != "" ) {
					setFilename(newFileName);
				}

				setLine(std::stoi(newLineNumber));

				setMacro(Macro("__LINE__", std::to_string(line)));
				setMacro(Macro("__FILE__", (filename)));

				return std::vector< std::string >();
			} else if( command == "define" ) {
				if( params.empty() ) {
					throw PreprocessorError(getLocation() + ": #define must be called with a macro name - it\'s arguments cannot be left empty.");
				}
				Index iterator;
				std::string macroName = "";
				std::vector< std::string > macroParameters = std::vector< std::string >();
				std::string macroDefinition = "";

				/**
				A state machine to parse the macro name, because we also have functional names.
				MACRO_NAME adds everything to the macroName variable. if it encounters a (, goes to PARAMETERS. if it encounters whitespace, breaks the loop
				PARAMETERS addse everything in the second word, seperating parameters by spaces
				*/
				enum STATE {
					MACRO_NAME, PARAMETERS
				};

				enum STATE parseState = MACRO_NAME;

				std::string currentParameter = "";

				//first we get the line number
				for( iterator = 0; iterator < params.length(); ++iterator ) {
					char value = params[iterator];

					if( parseState == MACRO_NAME ) {
						if( value == '(' ) {
							parseState = PARAMETERS;
							continue;
						} else if( std::isspace(value) ) {
							++iterator;//lets also increment the iterator so the space doesnt get added to the macroname
							break;//the second word, lets go to the second loop
						}

						macroName += value;

					} else if( parseState == PARAMETERS ) {
						//we dont want spaces in our parameters
						if( std::isspace(value) ) {
							continue;
						} else if( value == ')' ) {
							iterator += 2;//we add 2 to it. one to skip the ), and then another 1 to skip the space.
							macroParameters.push_back(currentParameter);
							break;
						} else if( value == ',' ) {
							macroParameters.push_back(currentParameter);
							currentParameter = "";
							continue;
						} else {
							currentParameter += value;
						}
					}

				}

				//remaining characters are added to the filename
				for( ; iterator < params.length(); ++iterator ) {
					macroDefinition += params[iterator];
				}

				defineMacro(Macro(macroName, macroDefinition, macroParameters, ""));

				return std::vector< std::string >();
			} else if( command == "undef" ) {
				if( params.empty() ) {
					throw PreprocessorError(getLocation() + ": #undef must be called with a macro name.");
				}

				Index iterator;
				std::string macroName = "";

				//first we get the line number
				for( iterator = 0; iterator < params.length(); ++iterator ) {
					if( std::isspace(params[iterator]) ) {
						++iterator;//lets also increment the iterator so the space doesnt get added to the filename
						break;//the second word, lets go to the second loop
					}
					macroName += params[iterator];
				}

				undefineMacro(params);

				return std::vector< std::string >();
			} else if( command == "include" ) {
				if( params.empty() ) {
					throw PreprocessorError(getLocation() + ": #include must be called with arguments seperated by either \"\'s, a macro, or <>");
				}


				if( params[0] == '<' ) {
					std::string term;
					//complete is whether both <> are found
					bool complete = false;

					for( Index iter = 1; iter < params.size(); ++iter ) {
						if( params[iter] == '>' ) {
							complete = true;
							break;
						} else {
							term += params[iter];
						}
					}

					if( !complete ) {
						throw PreprocessorError(getLocation() + ": Missing closing > in #include - " + params);
					}

					for( Index iter = 0; iter < includes.size(); ++iter ) {
						Include* incl = includes[iter];
						if( incl->hasFile(term) ) {
							std::string file = incl->getFile(term);
							//files include a lot of stuff. So we dynamically allocate the include preprocessor to prevent a stack overflow
							std::unique_ptr<Preprocessor> fileProcessor = std::unique_ptr<Preprocessor>(new Preprocessor(file, *this));

							fileProcessor->setMacro(Macro("__INCLUDE_LEVEL__", std::to_string(std::stoi(getMacro("__INCLUDE_LEVEL__").definition) + 1)));

							std::vector< std::string > tokens = fileProcessor->preprocessTokens();
							tokens.insert(tokens.begin(), "\n");

							//macros need to be added from the include. this works because the preprocessor inherits the previous macros
							macros = fileProcessor->macros;

							return tokens;
						}
					}

					//if it found the file, it would have returned it by now.
					throw PreprocessorError(getLocation() + ": Include with name " + term + " not found!");
				} else if( params[0] == '\"' ) {
					std::string term;
					//complete is whether both "" are found
					bool complete = false;

					for( Index iter = 1; iter < params.size(); ++iter ) {
						if( params[iter] == '\"' ) {
							complete = true;
							break;
						} else {
							term += params[iter];
						}
					}


					if( !complete ) {
						throw PreprocessorError(getLocation() + ": Missing closing \" in #include - " + params);
					}

					//check the current directory for file
					IncludeDirectory dir = IncludeDirectory(filename);

					if( dir.hasFile(term) ) {
						std::string file = dir.getFile(term);
						//files include a lot of stuff. So we dynamically allocate the include preprocessor to prevent a stack overflow
						std::unique_ptr<Preprocessor> fileProcessor = std::unique_ptr<Preprocessor>(new Preprocessor(file, *this));

						fileProcessor->setMacro(Macro("__INCLUDE_LEVEL__", std::to_string(std::stoi(getMacro("__INCLUDE_LEVEL__").definition) + 1)));

						std::vector< std::string > tokens = fileProcessor->preprocessTokens();
						tokens.insert(tokens.begin(), "\n");

						macros = fileProcessor->macros;

						return tokens;
					} else {
						throw PreprocessorError(getLocation() + ": file with name " + term + " not found!");
					}
				}

				return std::vector< std::string >();
				//glsl commands
			} else if( command == "version" || command == "extension" ) {
				//this is for shaders
				std::vector < std::string > output;
				output.push_back("#" + command + " " + params);

				return output;
			}
		}

		if( outputValue ) {
			throw PreprocessorError("Unknown directive: " + command);
		}

		return std::vector< std::string >();
	}

	int Preprocessor::getMacroLocation(const std::string & name) const {
		for( Index iterator = 0; iterator < macros.size(); ++iterator ) {
			if( macros[iterator].name == name ) {
				return iterator;
			}
		}
		return -1;
	}

	std::string Preprocessor::expandMacro(const std::string macro) const {
		if( !macro.empty() || (macro.length() == 1 && (std::isspace(macro[0]) || std::iscntrl(macro[0]) || std::isblank(macro[0]))) ) {

			std::string out = macro;

			bool stringify = false;

			//if we get input like foo(), we need to check if foo exists, not foo()
			Macro token = parseMacroName(macro);

			//remove whitespace to check for macro. a line like int a = MACRO wont work without this line, as it would be tokenized to be (a),( =), and ( MACRO)
			token.name.erase(std::remove_if(token.name.begin(), token.name.end(), ::isspace), token.name.end());

			if( token.name[0] == '#' ) {
				//get rid of the # so we can find the macro name
				token.name = token.name.substr(1);

				stringify = true;
			}

			const int macroLocation = getMacroLocation(token.name);
			if( macroLocation >= 0 ) {

				//the space is to prevent the execution of directives
				std::unique_ptr< Preprocessor > p = std::unique_ptr< Preprocessor >(new Preprocessor(' ' + macros[macroLocation].definition, *this));

				for( Index i = 0; i < token.parameters.size(); ++i ) {

					std::unique_ptr< Preprocessor > argumentProcessor = std::unique_ptr< Preprocessor >(new Preprocessor(token.parameters[i], *this));

					p->defineMacro(Macro(macros[macroLocation].parameters[i], argumentProcessor->preprocess()));
				}

				//the substr one is to get rid of the space we added in the previous line
				std::string replacement = p->preprocess().substr(1);

				if( stringify ) {
					//get rid of the # for stringication
					out.erase(std::remove_if(out.begin(), out.end(), [] (char c) {
						return c == '#';
					}), out.end());

					replacement = '\"' + replacement + '\"';
				}

				//these lines are to get rid of the parameters from the final output
				const size_t parametersLocation = out.find("(");
				if( parametersLocation != std::string::npos ) {
					out = out.substr(0, parametersLocation);
				}

				std::size_t index;

				//we want to preserve the whitespace in the final output, so this algorithim replaces the non-whitespace with the macro expansion
				while( (index = out.find(token.name)) != std::string::npos ) {
					out.replace(index, token.name.size(), replacement);
				}
			}//macroLocation>0
			else if( !token.parameters.empty() ) {

				//if its a function, we expand the parameters

				//this will expand macros in the parameters
				std::unique_ptr< Preprocessor > outProcessor = std::unique_ptr< Preprocessor >(new Preprocessor(token.parameterString, *this));

				for( Index i = 0; i < token.parameters.size(); i++ ) {

					std::unique_ptr< Preprocessor > argumentProcessor = std::unique_ptr< Preprocessor >(new Preprocessor(token.parameters[i], *this));

					std::string newArgument = argumentProcessor->preprocess();

					//if they are the same, a stack overflow happens
					if( newArgument != token.parameters[i] ) {
						outProcessor->setMacro(Macro(token.parameters[i], newArgument));
					}
				}

				std::string result = outProcessor->preprocess();

				//we need to preserve whitespace in the token, so we get it
				std::string whitespace;

				for( Index iter = 0; iter < macro.length(); ++iter ) {
					if( std::isspace(macro[iter]) ) {
						whitespace += macro[iter];
					} else {
						break;
					}
				}

				out = whitespace + token.name + '(' + result + ')';

			}//!token.parameters.empty()

			return out;

		}//!macro.empty

		return macro;
	}

	int Preprocessor::getIfScopeLocation() {
		int ifScope = getMacroLocation("__IF_SCOPE__");
		if( ifScope < 0 ) {
			ifScope = static_cast<int>(macros.size());
			setMacro(Macro("__IF_SCOPE__", "0"));
		}

		return ifScope;
	}

	int Preprocessor::getCurrentIfScopeLocation() {

		int currentIfScope = getMacroLocation("__CURRENT_IF_SCOPE__");
		if( currentIfScope < 0 ) {
			currentIfScope = static_cast<int>(macros.size());
			setMacro(Macro("__CURRENT_IF_SCOPE__", "0"));
		}

		return currentIfScope;
	}

	void Preprocessor::calculateIfScope(bool& outputValue, const bool statementPassed) {
		int ifScope = getIfScopeLocation();
		int currentIfScope = getCurrentIfScopeLocation();

		macros[ifScope].definition = std::to_string(std::stoi(macros[ifScope].definition) + 1);

		if( !statementPassed&&std::stoi(macros[currentIfScope].definition) == 0 ) {
			macros[currentIfScope].definition = macros[ifScope].definition;
			outputValue = false;
		}
	}

	bool Preprocessor::parseIfStatement(const std::string statement) {
		return false;
	}

	Macro Preprocessor::parseMacroName(const std::string & name) const {

		std::string macroName;
		std::string definition = "";
		std::vector < std::string > params = std::vector< std::string >();
		std::string parameterString = "";

		Index iter;

		int functionScope = 0;


		for( iter = 0; iter < name.length(); ++iter ) {
			char value = name[iter];
			if( value == '(' ) {

				//we want to ignore the first parenthsis
				++iter;

				//continue the loop and parse the arguments
				std::string currentParam = "";
				//each time a ( is encountered, functionScope is incremented. This is used to make sure all parameter functions are parsed correctly

				for( ; iter < name.length(); ++iter ) {
					char val = name[iter];

					if( val == '(' ) {
						++functionScope;
					}
					//function scope needs to be equal to 1, as that means there has been one set of parenthesis. functionScope==0 means its not a function
					else if( val == ','&&functionScope == 0 ) {
						params.push_back(currentParam);
						currentParam = "";
					} else if( val == ')' ) {
						if( functionScope == 0 ) {
							++iter;
							params.push_back(currentParam);
							break;
						} else {
							currentParam += val;
						}

						--functionScope;
					} else if( !std::isspace(val) ) {
						currentParam += val;
					}

					parameterString += val;
				}

				break;
			} else if( !std::isspace(value) ) {
				macroName += value;
			}
		}

		if( functionScope < 0 ) {
			throw PreprocessorError(getLocation() + ": Missing ( in function macro name - " + name);
		} else if( functionScope > 0 ) {
			throw PreprocessorError(getLocation() + ": Missing ) in function macro name - " + name);
		}

		for( ; iter < name.length(); ++iter ) {
			definition += name[iter];
		}

		//the substr of parameterString is to remove the ( at the beginning.
		return Macro(macroName, definition, params, parameterString);
	}

	void Preprocessor::setMacro(const Macro& m) {
		for( unsigned int iterator = 0; iterator < macros.size(); ++iterator ) {
			if( macros[iterator].name == m.name ) {
				macros[iterator].definition = m.definition;
				macros[iterator].parameters = m.parameters;
				return;
			}
		}
		macros.push_back(m);
	}


	void Preprocessor::defineMacro(const Macro& m) {
		for( unsigned int iterator = 0; iterator < sizeof(reservedWords) / sizeof(*reservedWords); ++iterator ) {
			if( reservedWords[iterator] == m.name ) {
				throw PreprocessorError(getLocation() + ": can\'t define " + m.name + " - it is a reserved word");
			}
		}

		setMacro(m);
	}

	bool Preprocessor::isMacroDefined(const std::string & name) const {
		for( Index iterator = 0; iterator < macros.size(); ++iterator ) {
			if( macros[iterator].name == name ) {
				return true;
			}
		}
		return false;
	}

	void Preprocessor::undefineMacro(const std::string & name) {
		Index iterator = 0;

		for( iterator = 0; iterator < macros.size(); ++iterator ) {
			if( macros[iterator].name == name ) {
				macros.erase(macros.begin() + iterator);
				return;
			}
		}

		//do nothing if its not found (standard says so)
	}

	Size Preprocessor::macroNumber() const {
		//size() returns size_t which could be larger than unsigned in on some systems, causing problems. static_cast will fix it
		return static_cast<Size>(macros.size());
	}

	void Preprocessor::defineStandardMacros() {

		setMacro(Macro("__BASE_FILE__", filename));
		setMacro(Macro("__FILE__", filename));

		//lets create a new block, so all of these horrendous variables disappear
		{

			std::time_t time = std::time(0);
			std::tm timeStruct;

			timeStruct = *os::localtime(&timeStruct, &time);

			char buffer[20];

			strftime(buffer, 80, "%D", &timeStruct);
			puts(buffer);

			setMacro(Macro("__DATE__", buffer));

			strftime(buffer, 80, "%T", &timeStruct);
			puts(buffer);

			setMacro(Macro("__TIME__", buffer));
		}

		//the following are macros denoted by the standard.
		MACE_PREDEFINE_MACRO(__STDC__);
		MACE_PREDEFINE_MACRO(__STDC_HOSTED__);
		MACE_PREDEFINE_MACRO(__STDC_MB_MIGHT_NEQ_WC__);
		MACE_PREDEFINE_MACRO(__STDC_ISO_10646__);
		MACE_PREDEFINE_MACRO(__STDC_VERSION__);
		MACE_PREDEFINE_MACRO(__STDC_IEC_559__);
		MACE_PREDEFINE_MACRO(__STDC_IEC_559_COMPLEX__);
		MACE_PREDEFINE_MACRO(__STDCPP_STRICT_POINTER_SAFETY__);

		//now, a giant list of predefined macros from various compilers, including gcc, clang, borland, oracle, digital mars, and vsc.
		//this list does not include compiler specific macros (denoted by a prefix, like __GCC_VERSION__)
		MACE_PREDEFINE_MACRO(__CHAR_UNSIGNED__);
		MACE_PREDEFINE_MACRO(__CHAR_UNSIGNED);
		MACE_PREDEFINE_MACRO(__WCHAR_UNSIGNED__);
		MACE_PREDEFINE_MACRO(__SIZE_TYPE__);
		MACE_PREDEFINE_MACRO(__PTRDIFF_TYPE__);
		MACE_PREDEFINE_MACRO(__WCHAR_TYPE__);
		MACE_PREDEFINE_MACRO(__WINT_TYPE__);
		MACE_PREDEFINE_MACRO(__INTMAX_TYPE__);
		MACE_PREDEFINE_MACRO(__UINTMAX_TYPE__);
		MACE_PREDEFINE_MACRO(__SIG_ATOMIC_TYPE__);
		MACE_PREDEFINE_MACRO(__INT8_TYPE__);
		MACE_PREDEFINE_MACRO(__INT16_TYPE__);
		MACE_PREDEFINE_MACRO(__INT32_TYPE__);
		MACE_PREDEFINE_MACRO(__INT64_TYPE__);
		MACE_PREDEFINE_MACRO(__UINT8_TYPE__);
		MACE_PREDEFINE_MACRO(__UINT16_TYPE__);
		MACE_PREDEFINE_MACRO(__UINT32_TYPE__);
		MACE_PREDEFINE_MACRO(__UINT64_TYPE__);
		MACE_PREDEFINE_MACRO(__INT_LEAST8_TYPE__);
		MACE_PREDEFINE_MACRO(__INT_LEAST16_TYPE__);
		MACE_PREDEFINE_MACRO(__INT_LEAST32_TYPE__);
		MACE_PREDEFINE_MACRO(__INT_LEAST64_TYPE__);
		MACE_PREDEFINE_MACRO(__UINT_LEAST8_TYPE__);
		MACE_PREDEFINE_MACRO(__UINT_LEAST16_TYPE__);
		MACE_PREDEFINE_MACRO(__UINT_LEAST32_TYPE__);
		MACE_PREDEFINE_MACRO(__UINT_LEAST64_TYPE__);
		MACE_PREDEFINE_MACRO(__INT_FAST8_TYPE__);
		MACE_PREDEFINE_MACRO(__INT_FAST16_TYPE__);
		MACE_PREDEFINE_MACRO(__INT_FAST32_TYPE__);
		MACE_PREDEFINE_MACRO(__INT_FAST64_TYPE__);
		MACE_PREDEFINE_MACRO(__UINT_FAST8_TYPE__);
		MACE_PREDEFINE_MACRO(__UINT_FAST16_TYPE__);
		MACE_PREDEFINE_MACRO(__UINT_FAST32_TYPE__);
		MACE_PREDEFINE_MACRO(__UINT_FAST64_TYPE__);
		MACE_PREDEFINE_MACRO(__INTPTR_TYPE__);
		MACE_PREDEFINE_MACRO(__UINTPTR_TYPE__);
		MACE_PREDEFINE_MACRO(__STDCPP_THREADS__);
		MACE_PREDEFINE_MACRO(__cplusplus);
		MACE_PREDEFINE_MACRO(__CHAR_BIT__);
		MACE_PREDEFINE_MACRO(__INT_SHORT__);
		MACE_PREDEFINE_MACRO(__SCHAR_MAX__);
		MACE_PREDEFINE_MACRO(__SHRT_MAX__);
		MACE_PREDEFINE_MACRO(__INT_MAX__);
		MACE_PREDEFINE_MACRO(__LONG_MAX__);
		MACE_PREDEFINE_MACRO(__LONG_LONG_MAX__);
		MACE_PREDEFINE_MACRO(__INTMAX_MAX__);
		MACE_PREDEFINE_MACRO(_DEBUG);
		MACE_PREDEFINE_MACRO(_INTEGRAL_MAX_BITS);
		MACE_PREDEFINE_MACRO(__LP64__);
		MACE_PREDEFINE_MACRO(_LP64);
		MACE_PREDEFINE_MACRO(__OBJC__);
		MACE_PREDEFINE_MACRO(__ASSEMBLER__);
		MACE_PREDEFINE_MACRO(__DEPRECATED);
		MACE_PREDEFINE_MACRO(__USER_LABEL_PREFIX__);
		MACE_PREDEFINE_MACRO(__REGISTER_PREFIX__);
		MACE_PREDEFINE_MACRO(__ELF__);
		MACE_PREDEFINE_MACRO(__OPTIMIZE__);
		MACE_PREDEFINE_MACRO(__OPTIMIZE_SIZE__);
		MACE_PREDEFINE_MACRO(__NO_INLINE__);
		MACE_PREDEFINE_MACRO(__NO_MATH_ERRNO__);
		MACE_PREDEFINE_MACRO(__FP_FAST_FMA);
		MACE_PREDEFINE_MACRO(__FP_FAST_FMAF);
		MACE_PREDEFINE_MACRO(__FP_FAST_FMAL);
		MACE_PREDEFINE_MACRO(__USING_SJLJ_EXCEPTIONS__);
		MACE_PREDEFINE_MACRO(__EXCEPTIONS);
		MACE_PREDEFINE_MACRO(__FLOAT_WORD_ORDER__);
		MACE_PREDEFINE_MACRO(__SCHAR_MAX__);
		MACE_PREDEFINE_MACRO(__WCHAR_MAX__);
		MACE_PREDEFINE_MACRO(__SHRT_MAX__);
		MACE_PREDEFINE_MACRO(__INT_MAX__);
		MACE_PREDEFINE_MACRO(__LONG_MAX__);
		MACE_PREDEFINE_MACRO(__LONG_LONG_MAX__);
		MACE_PREDEFINE_MACRO(__WINT_MAX__);
		MACE_PREDEFINE_MACRO(__SIZE_MAX__);
		MACE_PREDEFINE_MACRO(__PTRDIFF_MAX__);
		MACE_PREDEFINE_MACRO(__INTMAX_MAX__);
		MACE_PREDEFINE_MACRO(__UINTMAX_MAX__);
		MACE_PREDEFINE_MACRO(__SIG_ATOMIC_MAX__);
		MACE_PREDEFINE_MACRO(__INT8_MAX__);
		MACE_PREDEFINE_MACRO(__INT16_MAX__);
		MACE_PREDEFINE_MACRO(__INT32_MAX__);
		MACE_PREDEFINE_MACRO(__INT64_MAX__);
		MACE_PREDEFINE_MACRO(__UINT8_MAX__);
		MACE_PREDEFINE_MACRO(__UINT16_MAX__);
		MACE_PREDEFINE_MACRO(__UINT32_MAX__);
		MACE_PREDEFINE_MACRO(__UINT64_MAX__);
		MACE_PREDEFINE_MACRO(__INT_LEAST8_MAX__);
		MACE_PREDEFINE_MACRO(__INT_LEAST16_MAX__);
		MACE_PREDEFINE_MACRO(__INT_LEAST32_MAX__);
		MACE_PREDEFINE_MACRO(__INT_LEAST64_MAX__);
		MACE_PREDEFINE_MACRO(__UINT_LEAST8_MAX__);
		MACE_PREDEFINE_MACRO(__UINT_LEAST16_MAX__);
		MACE_PREDEFINE_MACRO(__UINT_LEAST32_MAX__);
		MACE_PREDEFINE_MACRO(__UINT_LEAST64_MAX__);
		MACE_PREDEFINE_MACRO(__INT_FAST8_MAX__);
		MACE_PREDEFINE_MACRO(__INT_FAST16_MAX__);
		MACE_PREDEFINE_MACRO(__INT_FAST32_MAX__);
		MACE_PREDEFINE_MACRO(__INT_FAST64_MAX__);
		MACE_PREDEFINE_MACRO(__UINT_FAST8_MAX__);
		MACE_PREDEFINE_MACRO(__UINT_FAST16_MAX__);
		MACE_PREDEFINE_MACRO(__UINT_FAST32_MAX__);
		MACE_PREDEFINE_MACRO(__UINT_FAST64_MAX__);
		MACE_PREDEFINE_MACRO(__INTPTR_MAX__);
		MACE_PREDEFINE_MACRO(__UINTPTR_MAX__);
		MACE_PREDEFINE_MACRO(__WCHAR_MIN__);
		MACE_PREDEFINE_MACRO(__WINT_MIN__);
		MACE_PREDEFINE_MACRO(__SIG_ATOMIC_MIN__);
		MACE_PREDEFINE_MACRO(__INT8_C);
		MACE_PREDEFINE_MACRO(__INT16_C);
		MACE_PREDEFINE_MACRO(__INT32_C);
		MACE_PREDEFINE_MACRO(__INT64_C);
		MACE_PREDEFINE_MACRO(__UINT8_C);
		MACE_PREDEFINE_MACRO(__UINT16_C);
		MACE_PREDEFINE_MACRO(__UINT32_C);
		MACE_PREDEFINE_MACRO(__UINT64_C);
		MACE_PREDEFINE_MACRO(__INTMAX_C);
		MACE_PREDEFINE_MACRO(__UINTMAX_C);
		MACE_PREDEFINE_MACRO(__SSP__);
		MACE_PREDEFINE_MACRO(__SSP_ALL__);
		MACE_PREDEFINE_MACRO(__SSP_STRONG__);
		MACE_PREDEFINE_MACRO(__SSP_EXPLICIT__);
		MACE_PREDEFINE_MACRO(__SANITIZE_ADDRESS__);
		MACE_PREDEFINE_MACRO(__SANITIZE_THREAD__);
		MACE_PREDEFINE_MACRO(__NO_MATH_ERRNO__);
		MACE_PREDEFINE_MACRO(__FP_FAST_FMA);
		MACE_PREDEFINE_MACRO(__FP_FAST_FMAF);
		MACE_PREDEFINE_MACRO(__FP_FAST_FMAL);
		MACE_PREDEFINE_MACRO(__NEXT_RUNTIME__);
		MACE_PREDEFINE_MACRO(__STRICT_ANSI__);
		MACE_PREDEFINE_MACRO(__BYTE_ORDER__);
		MACE_PREDEFINE_MACRO(__ORDER_LITTLE_ENDIAN__);
		MACE_PREDEFINE_MACRO(__ORDER_BIG_ENDIAN__);
		MACE_PREDEFINE_MACRO(__ORDER_PDP_ENDIAN__);
		MACE_PREDEFINE_MACRO(__SCHAR_WIDTH__);
		MACE_PREDEFINE_MACRO(__SHRT_WIDTH__);
		MACE_PREDEFINE_MACRO(__INT_WIDTH__);
		MACE_PREDEFINE_MACRO(__LONG_WIDTH__);
		MACE_PREDEFINE_MACRO(__LONG_LONG_WIDTH__);
		MACE_PREDEFINE_MACRO(__PTRDIFF_WIDTH__);
		MACE_PREDEFINE_MACRO(__SIG_ATOMIC_WIDTH__);
		MACE_PREDEFINE_MACRO(__SIZE_WIDTH__);
		MACE_PREDEFINE_MACRO(__WCHAR_WIDTH__);
		MACE_PREDEFINE_MACRO(__WINT_WIDTH__);
		MACE_PREDEFINE_MACRO(__INT_LEAST8_WIDTH__);
		MACE_PREDEFINE_MACRO(__INT_LEAST16_WIDTH__);
		MACE_PREDEFINE_MACRO(__INT_LEAST32_WIDTH__);
		MACE_PREDEFINE_MACRO(__INT_LEAST64_WIDTH__);
		MACE_PREDEFINE_MACRO(__INT_FAST8_WIDTH__);
		MACE_PREDEFINE_MACRO(__INT_FAST16_WIDTH__);
		MACE_PREDEFINE_MACRO(__INT_FAST32_WIDTH__);
		MACE_PREDEFINE_MACRO(__INT_FAST64_WIDTH__);
		MACE_PREDEFINE_MACRO(__INTPTR_WIDTH__);
		MACE_PREDEFINE_MACRO(__INTMAX_WIDTH__);
		MACE_PREDEFINE_MACRO(__SIZEOF_INT__);
		MACE_PREDEFINE_MACRO(__SIZEOF_LONG__);
		MACE_PREDEFINE_MACRO(__SIZEOF_LONG_LONG__);
		MACE_PREDEFINE_MACRO(__SIZEOF_SHORT__);
		MACE_PREDEFINE_MACRO(__SIZEOF_POINTER__);
		MACE_PREDEFINE_MACRO(__SIZEOF_FLOAT__);
		MACE_PREDEFINE_MACRO(__SIZEOF_DOUBLE__);
		MACE_PREDEFINE_MACRO(__SIZEOF_LONG_DOUBLE__);
		MACE_PREDEFINE_MACRO(__SIZEOF_SIZE_T__);
		MACE_PREDEFINE_MACRO(__SIZEOF_WCHAR_T__);
		MACE_PREDEFINE_MACRO(__SIZEOF_WINT_T__);
		MACE_PREDEFINE_MACRO(__SIZEOF_PTRDIFF_T__);
		MACE_PREDEFINE_MACRO(__ATOM__);
		MACE_PREDEFINE_MACRO(__AVX__);
		MACE_PREDEFINE_MACRO(__AVX2__);
		MACE_PREDEFINE_MACRO(_CONTROL_FLOW_GUARD);
		MACE_PREDEFINE_MACRO(__cplusplus_cli);
		MACE_PREDEFINE_MACRO(__cplusplus_winrt);
		MACE_PREDEFINE_MACRO(_CPPRTTI);
		MACE_PREDEFINE_MACRO(_CPPUNWIND);
		MACE_PREDEFINE_MACRO(_DLL);
		MACE_PREDEFINE_MACRO(__INTELLISENSE__);
		MACE_PREDEFINE_MACRO(_ISO_VOLATILE);
		MACE_PREDEFINE_MACRO(_KERNEL_MODE);
		MACE_PREDEFINE_MACRO(_WINRT_DLL);
		MACE_PREDEFINE_MACRO(_WCHAR_T_DEFINED);
		MACE_PREDEFINE_MACRO(_VC_NODEFAULTLIB);
		MACE_PREDEFINE_MACRO(_PREFAST_);
		MACE_PREDEFINE_MACRO(_OPENMP);
		MACE_PREDEFINE_MACRO(_NATIVE_WCHAR_T_DEFINED);
		MACE_PREDEFINE_MACRO(_MT);
		MACE_PREDEFINE_MACRO(_MANAGED);
		MACE_PREDEFINE_MACRO(_WCTYPE_T_DEFINED);
		MACE_PREDEFINE_MACRO(_PLACEMENT_DELETE);
		MACE_PREDEFINE_MACRO(_PGO_INSTRUMENT);
		MACE_PREDEFINE_MACRO(_BOOL);
		MACE_PREDEFINE_MACRO(__WINT_TYPE__);
		MACE_PREDEFINE_MACRO(__SSSE3__);
		MACE_PREDEFINE_MACRO(__SSE4_2__);
		MACE_PREDEFINE_MACRO(__SSE4_1__);
		MACE_PREDEFINE_MACRO(__SSE3__);
		MACE_PREDEFINE_MACRO(__SSE2__);
		MACE_PREDEFINE_MACRO(__SSE__);
		MACE_PREDEFINE_MACRO(__SIZE_TYPE__);
		MACE_PREDEFINE_MACRO(__SIGNED_CHARS__);
		MACE_PREDEFINE_MACRO(__QMSPP_);
		MACE_PREDEFINE_MACRO(__PIC__);
		MACE_PREDEFINE_MACRO(__pic__);
		MACE_PREDEFINE_MACRO(__NO_MATH_INLINES);
		MACE_PREDEFINE_MACRO(__NO_STRING_INLINES);
		MACE_PREDEFINE_MACRO(__MMX__);
		MACE_PREDEFINE_MACRO(__MIC__);
		MACE_PREDEFINE_MACRO(__LONG_DOUBLE_64__);
		MACE_PREDEFINE_MACRO(__LONG_DOUBLE_SIZE__);
		MACE_PREDEFINE_MACRO(i386);
		MACE_PREDEFINE_MACRO(__i386__);
		MACE_PREDEFINE_MACRO(__i386);
		MACE_PREDEFINE_MACRO(__HONOR_STD);
		MACE_PREDEFINE_MACRO(__GFX__);
		MACE_PREDEFINE_MACRO(__extension__);
		MACE_PREDEFINE_MACRO(__EDG__);
		MACE_PREDEFINE_MACRO(__EDG_VERSION__);
		MACE_PREDEFINE_MACRO(__DYNAMIC__);
		MACE_PREDEFINE_MACRO(__DEPRECATED);
		MACE_PREDEFINE_MACRO(__cilk);
		MACE_PREDEFINE_MACRO(__ARRAY_OPERATORS);
		MACE_PREDEFINE_MACRO(__AVX__);
		MACE_PREDEFINE_MACRO(__AVX2__);
		MACE_PREDEFINE_MACRO(__AVX512BW__);
		MACE_PREDEFINE_MACRO(__AVX512CD__);
		MACE_PREDEFINE_MACRO(__AVX512DQ__);
		MACE_PREDEFINE_MACRO(__AVX512ER__);
		MACE_PREDEFINE_MACRO(__AVX512F__);
		MACE_PREDEFINE_MACRO(__AVX512PF__);
		MACE_PREDEFINE_MACRO(__AVX512VL__);
		MACE_PREDEFINE_MACRO(__ARRAYNEW);
		MACE_PREDEFINE_MACRO(__spar);
		MACE_PREDEFINE_MACRO(sparc);
		MACE_PREDEFINE_MACRO(__sparcv);
		MACE_PREDEFINE_MACRO(__SVR4);
		MACE_PREDEFINE_MACRO(_ENABLE_ARRAYNEW);
		MACE_PREDEFINE_MACRO(_PUSHPOP_SUPPORTED);
		MACE_PREDEFINE_MACRO(__INTSIZE);
		MACE_PREDEFINE_MACRO(__DEFALIGN);
		MACE_PREDEFINE_MACRO(__INLINE_8087);
		MACE_PREDEFINE_MACRO(__I86__);
		MACE_PREDEFINE_MACRO(__NT__);
		MACE_PREDEFINE_MACRO(__SC__);
		MACE_PREDEFINE_MACRO(__ZTC__);
		MACE_PREDEFINE_MACRO(DEBUG);
		MACE_PREDEFINE_MACRO(__FPCE__IEEE__);
		MACE_PREDEFINE_MACRO(__FPCE__);
		MACE_PREDEFINE_MACRO(__VCM__);
		MACE_PREDEFINE_MACRO(__SMALL__);
		MACE_PREDEFINE_MACRO(__MEDIUM__);
		MACE_PREDEFINE_MACRO(__COMPACT__);
		MACE_PREDEFINE_MACRO(__LARGE__);
	}//defineStandardMacros

	void Preprocessor::defineOSMacros() {
		/*
		The following is a list of macros for many operating systems, most of which are probably not in use today.
		I still included them here just to be safe.
		I never know where this class can end up, so why not?

		This list was compiled from the macros located at https://sourceforge.net/p/predef/wiki/OperatingSystems/
		Don't know how they did it, but it exists
		*/

		MACE_PREDEFINE_MACRO(_AIX);
		MACE_PREDEFINE_MACRO(__TOS_AIX__);
		MACE_PREDEFINE_MACRO(__ANDROID__);
		MACE_PREDEFINE_MACRO(__ANDROID_API__);
		MACE_PREDEFINE_MACRO(UTS);
		MACE_PREDEFINE_MACRO(AMIGA);
		MACE_PREDEFINE_MACRO(__amigaos__);
		MACE_PREDEFINE_MACRO(aegis);
		MACE_PREDEFINE_MACRO(apollo);
		MACE_PREDEFINE_MACRO(__BEOS__);
		MACE_PREDEFINE_MACRO(__bg__);
		MACE_PREDEFINE_MACRO(__bgq__);
		MACE_PREDEFINE_MACRO(__THW_BLUEGENE__);
		MACE_PREDEFINE_MACRO(__TOS_BGQ__);
		MACE_PREDEFINE_MACRO(__FreeBSD__);
		MACE_PREDEFINE_MACRO(__NetBSD__);
		MACE_PREDEFINE_MACRO(__OpenBSD__);
		MACE_PREDEFINE_MACRO(__bsdi__);
		MACE_PREDEFINE_MACRO(__DragonFly__);
		MACE_PREDEFINE_MACRO(BSD);
		MACE_PREDEFINE_MACRO(BSD4_2);
		MACE_PREDEFINE_MACRO(BSD4_3);
		MACE_PREDEFINE_MACRO(BSD4_4);
		MACE_PREDEFINE_MACRO(_SYSTYPE_BSD);
		MACE_PREDEFINE_MACRO(__bsdi__);
		MACE_PREDEFINE_MACRO(__convex__);
		MACE_PREDEFINE_MACRO(__CYGWIN__);
		MACE_PREDEFINE_MACRO(DGUX);
		MACE_PREDEFINE_MACRO(__DGUX__);
		MACE_PREDEFINE_MACRO(__dgux__);
		MACE_PREDEFINE_MACRO(__DragonFly__);
		MACE_PREDEFINE_MACRO(_SEQUENT_);
		MACE_PREDEFINE_MACRO(sequent);
		MACE_PREDEFINE_MACRO(__ECOS);
		MACE_PREDEFINE_MACRO(__EMX__);
		MACE_PREDEFINE_MACRO(__FreeBSD__);
		MACE_PREDEFINE_MACRO(__FreeBSD_kernel__);
		MACE_PREDEFINE_MACRO(__FreeBSD_version);
		MACE_PREDEFINE_MACRO(__GNU__);
		MACE_PREDEFINE_MACRO(__gnu_hurd__);
		MACE_PREDEFINE_MACRO(__GLIBC__);
		MACE_PREDEFINE_MACRO(__gnu_linux__);
		MACE_PREDEFINE_MACRO(__hiuxmpp);
		MACE_PREDEFINE_MACRO(_hpux);
		MACE_PREDEFINE_MACRO(hpux);
		MACE_PREDEFINE_MACRO(__hpux);
		MACE_PREDEFINE_MACRO(__OS400__);
		MACE_PREDEFINE_MACRO(__INTEGRITY);
		MACE_PREDEFINE_MACRO(__INTERIX);
		MACE_PREDEFINE_MACRO(sgi);
		MACE_PREDEFINE_MACRO(__sgi);
		MACE_PREDEFINE_MACRO(__linux__);
		MACE_PREDEFINE_MACRO(linux);
		MACE_PREDEFINE_MACRO(__linux);
		MACE_PREDEFINE_MACRO(__Lynx__);
		MACE_PREDEFINE_MACRO(macintosh);
		MACE_PREDEFINE_MACRO(Macintosh);
		MACE_PREDEFINE_MACRO(__APPLE__);
		MACE_PREDEFINE_MACRO(__MACH__);
		MACE_PREDEFINE_MACRO(__OS9000);
		MACE_PREDEFINE_MACRO(__OSK);
		MACE_PREDEFINE_MACRO(__minix);
		MACE_PREDEFINE_MACRO(__MORPHOS__);
		MACE_PREDEFINE_MACRO(mpeix);
		MACE_PREDEFINE_MACRO(__mpxel);
		MACE_PREDEFINE_MACRO(MSDOS);
		MACE_PREDEFINE_MACRO(__MSDOS__);
		MACE_PREDEFINE_MACRO(_MSDOS);
		MACE_PREDEFINE_MACRO(__DOS__);
		MACE_PREDEFINE_MACRO(__NetBSD_Version__);
		MACE_PREDEFINE_MACRO(__TANDEM);
		MACE_PREDEFINE_MACRO(__nucleus__);
		MACE_PREDEFINE_MACRO(OS2);
		MACE_PREDEFINE_MACRO(_OS2);
		MACE_PREDEFINE_MACRO(__OS2__);
		MACE_PREDEFINE_MACRO(__TOS_OS2__);
		MACE_PREDEFINE_MACRO(__palmos__);
		MACE_PREDEFINE_MACRO(EPLAN9);
		MACE_PREDEFINE_MACRO(pyr);
		MACE_PREDEFINE_MACRO(__QNX__);
		MACE_PREDEFINE_MACRO(__QNXTO__);
		MACE_PREDEFINE_MACRO(_NTO_VERSION);
		MACE_PREDEFINE_MACRO(BBNDK_VERSION_CURRENT);
		MACE_PREDEFINE_MACRO(sinux);
		MACE_PREDEFINE_MACRO(M_I386);
		MACE_PREDEFINE_MACRO(M_XENIX);
		MACE_PREDEFINE_MACRO(_SCO_DS);
		MACE_PREDEFINE_MACRO(sun);
		MACE_PREDEFINE_MACRO(__sun);
		MACE_PREDEFINE_MACRO(__VOS__);
		MACE_PREDEFINE_MACRO(__sysv__);
		MACE_PREDEFINE_MACRO(__SVR4);
		MACE_PREDEFINE_MACRO(__svr4__);
		MACE_PREDEFINE_MACRO(_SYSTYPE_SVR4);
		MACE_PREDEFINE_MACRO(__SYLLABLE__);
		MACE_PREDEFINE_MACRO(__SYMBIAN32__);
		MACE_PREDEFINE_MACRO(__osf__);
		MACE_PREDEFINE_MACRO(__osf);
		MACE_PREDEFINE_MACRO(ultrix);
		MACE_PREDEFINE_MACRO(__ultrix);
		MACE_PREDEFINE_MACRO(__ultrix__);
		MACE_PREDEFINE_MACRO(vax);
		MACE_PREDEFINE_MACRO(_UNICOS);
		MACE_PREDEFINE_MACRO(_CRAY);//thats so cray cray
		MACE_PREDEFINE_MACRO(__crayx1);
		MACE_PREDEFINE_MACRO(__unix__);
		MACE_PREDEFINE_MACRO(__unix);
		MACE_PREDEFINE_MACRO(sco);
		MACE_PREDEFINE_MACRO(_UNIXWARE7);
		MACE_PREDEFINE_MACRO(_UWIN);
		MACE_PREDEFINE_MACRO(VMS);
		MACE_PREDEFINE_MACRO(__VMS);
		MACE_PREDEFINE_MACRO(__VMS_VER);
		MACE_PREDEFINE_MACRO(__VXWORKS__);
		MACE_PREDEFINE_MACRO(__vxworks);
		MACE_PREDEFINE_MACRO(__RTP__);
		MACE_PREDEFINE_MACRO(_WRS_KERNEL);
		MACE_PREDEFINE_MACRO(_WIN16);
		MACE_PREDEFINE_MACRO(_WIN32);
		MACE_PREDEFINE_MACRO(_WIN64);
		MACE_PREDEFINE_MACRO(__WIN32__);
		MACE_PREDEFINE_MACRO(__TOS_WIN__);
		MACE_PREDEFINE_MACRO(__WINDOWS__);
		MACE_PREDEFINE_MACRO(_WIN32_WCE);
		MACE_PREDEFINE_MACRO(_WINDU_SOURCE);
		MACE_PREDEFINE_MACRO(__MVS__);
		MACE_PREDEFINE_MACRO(__HOS_MVS__);
		MACE_PREDEFINE_MACRO(__TOS_MVS__);
	}

	const Macro & Preprocessor::getMacro(const std::string & name) const {
		for( unsigned int iterator = 0; iterator < macros.size(); ++iterator ) {
			if( macros[iterator].name == name ) {
				return macros[iterator];
			}
		}
		throw PreprocessorError(getLocation() + ": Unknown macro " + name);
	}

	std::string Preprocessor::getLocation() const {
		return "Line " + std::to_string(line) + " in " + (filename);
	}

	void Preprocessor::addInclude(Include & include) {
		includes.push_back(&include);
	}

	std::vector< Include* > Preprocessor::getIncludes() {
		return includes;
	}

	const std::vector< Include* > Preprocessor::getIncludes() const {
		return includes;
	}

	void Preprocessor::setIncludes(const std::vector< Include* > include) {
		this->includes = include;
	}

	const std::string Preprocessor::getInput() const {
		return input;
	}

	std::string Preprocessor::getFilename() {
		return filename;
	}

	const std::string Preprocessor::getFilename() const {
		return filename;
	}

	void Preprocessor::setFilename(const std::string & file) {
		this->filename = file;
	}

	unsigned int Preprocessor::getLine() {
		return line;
	}

	const unsigned int Preprocessor::getLine() const {
		return line;
	}

	void Preprocessor::setLine(const unsigned int lineNumber) {
		this->line = lineNumber;
	}

	//d is part of std? how lewd can we get
	Macro::Macro(std::string n, std::string d, std::vector<std::string> p, std::string ps) : name(n), definition(d), parameters(p), parameterString(ps) {}

	Macro::Macro(std::string n, std::string d) : Macro(n, d, std::vector< std::string >(), "") {}

	IncludeString::IncludeString(const std::string & c, const std::string n) : content(c), name(n) {}

	bool IncludeString::hasFile(const std::string & fileName) const {
		return this->name == fileName;
	}

	std::string IncludeString::getFile(const std::string & fileName) const {
		return this->name == fileName ? content : "";
	}

	IncludeDirectory::IncludeDirectory(const std::string & dir) : directory(dir) {}

	bool IncludeDirectory::hasFile(const std::string & name) const {

#if defined(MACE_WINDOWS)
		std::ifstream f(directory + '\\' + name);
#else
		std::ifstream f(directory + '/' + name);
#endif
		return f.good();
	}

	std::string IncludeDirectory::getFile(const std::string & name) const {

		std::string fileName = directory;

#if defined(MACE_WINDOWS)
		fileName += '\\';
#else
		fileName += '/';
#endif
		fileName += name;

		std::ifstream f(fileName);

		std::string out = "";

		std::string line = "";
		if( f.is_open() ) {
			while( std::getline(f, line) ) {
				out += line;
				out += '\n';
			}
		} else {
			throw PreprocessorError("File with name " + name + "doesn\'t exist!");
		}

		f.close();

		return "#line 0 " + fileName + "\n" + out;
	}

}//mc

//just to be safe. wont hurt to undefine at the end
#undef MACE_LOCALTIME
#undef MACE_STRINGIFY
#undef MACE_NAME_STRINGIFY
#undef MACE_PREDEFINE_MACRO
