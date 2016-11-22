/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_UTILITY_PREPROCESSOR_H
#define MACE_UTILITY_PREPROCESSOR_H

#include <vector>
#include <string>
#include <map>
#include <MACE/System/Exceptions.h>


namespace mc {

	/**
	`Exception` thrown when a syntax error is encountered, or \#error is called.
	*/
	using PreprocessorException = Exception;

	/**
	Represents a `Macro` in code. Created automatically when #define is encountered.
	@see Preprocessor
	*/
	struct Macro {
		/**
		Generates a function macro
		@param name The name of the macro, not including parenthesis
		@param definition What it expands to. Parameters can be used here.
		@param parameters A list of every parameter. Spaces parenthesis should not be included.
		@param parameterString The original string with the parenthesis, spaces, and parameters intact.
		@see Macro(std::string, std::string)
		*/
		Macro(std::string name, std::string definition, std::vector < std::string > parameters, std::string parameterString);
		/**
		Generates a standard, variable like macro
		@param name The name of the macro. Whenever this is encountered in the input, it will be expanded.
		@param definition What the macro expands to.
		*/
		Macro(std::string name, std::string definition);

		/**
		The name of the macro, as specified by the constructor.
		@see definition
		*/
		std::string name;
		/**
		The definition of the macro. Whenever `name` is encountered, it will be expanded to the definition.
		@see name
		*/
		std::string definition;
		/**
		If the macro is a function, list of parameters that the macro takes in. Can be empty if not a function.
		@see parameterString
		*/
		std::vector < std::string > parameters;
		/**
		The original string with the parenthesis, spaces, and parameters intact, but without the name
		*/
		std::string parameterString;
	};

	/**
	Superclass for classes that represent items that can be included via #include.
	@see IncludeString
	@see IncludeDirectory
	@see Preprocessor
	*/
	class Include {
	public:
		/**
		Checks whether this include has the specified file. If there isn't an `Include` with the file, then a `PreprocessorException` is thrown.
		*/
		virtual bool hasFile(const std::string& name) const = 0;
		/**
		Retrieves a file with a certain name.
		*/
		virtual std::string getFile(const std::string& name) const = 0;
	};

	/**
	Represents a fake file, in which you can add include files that don't exist on the filesystem.
	@see IncludeDirectory
	@see Preprocessor
	*/
	class IncludeString: public Include {
	public:
		/**
		Creates an `IncludeString` with the specified info.
		@param content What the `IncludeString` returns when included
		@param name The name of the `IncludeString` which is used by #include
		*/
		IncludeString(const std::string& content, const std::string name);

		bool hasFile(const std::string& name) const;
		std::string getFile(const std::string& name) const;

	private:
		const std::string content = "";
		const std::string name = "";
	};

	/**
	Represents a directory on the disk from which you can include files from.
	@see IncludeString
	@see Preprocessor
	*/
	class IncludeDirectory: public Include {
	public:
		/**
		Creates an `IncludeDirectory` representing a directory on the filesystem.
		@param dir Path to a directory
		*/
		IncludeDirectory(const std::string& dir);

		bool hasFile(const std::string& name) const;
		std::string getFile(const std::string& name) const;

	private:

		const std::string directory = "";
	};

	/**
	Customizable string preprocessor which follows ANSI-C preprocessor standard.
	<p>
	It can define all standard and operating system macros with special functions. Additionally, the following macros are always defined:
	| Macro name           | Macro definition                                                                                                                   |
	|----------------------|------------------------------------------------------------------------------------------------------------------------------------|
	| __MACE__             | Always defined                                                                                                                     |
	| __INCLUDE_LEVEL__    | Incremented by 1 when a file is included, and decremented when the included file ends. Identical to the GCC macro of the same name |
	| __IF_SCOPE__         | The depth of the current preprocessor conditional.  Shouldn't be used.                                                             |
	| __CURRENT_IF_SCOPE__ | The level at which a conditional directive returned false. Shouldn't be used.                                                      |

	Even if they are not defined, you can never defined or undefine a standard macro as denoted by the ANSI-C standard. You also can't define or undefine keywords
	<p>
	Every directive in the ANSI-C standard is supported except for #if, #elif, and #pragma. #warning outputs a message to console. Additionally, #version is ignored for GLSL support.

	@see Macro
	@see Include
	@todo Digraphs (see 6.4.6 of standard)
	@todo Variable arguments
	@todo Macro concat
	@todo #if and #elif
	@todo #pragma (if STDC follows, do no macro replacement)
	*/
	class Preprocessor {
	public:
		Preprocessor(const std::string& input, const std::string& filename = "Unknown file");
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
		unsigned int macroNumber() const;

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

		std::vector< std::string > parse();

		std::vector< std::string > executeDirective(bool& outputValue, const std::string& command, const std::string& params);

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

	};//Preprocessor
}//mc

#endif