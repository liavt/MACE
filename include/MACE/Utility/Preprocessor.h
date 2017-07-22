/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__UTILITY_PREPROCESSOR_H
#define MACE__UTILITY_PREPROCESSOR_H

#include <MACE/Core/Constants.h>
#include <MACE/Core/Error.h>

#include <vector>
#include <string>
#include <map>


namespace mc {

	/**
	`Exception` thrown when a syntax error is encountered, or #error is called. The filename and line number will be in message.
	*/
	MACE__DECLARE_ERROR(Preprocessor);

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
	Superclass for classes that represent items that can be included via `#include.`
	@see IncludeString
	@see IncludeDirectory
	@see Preprocessor
	*/
	class Include {
	public:
		virtual ~Include() = default;

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

		bool hasFile(const std::string& name) const override;
		std::string getFile(const std::string& name) const override;

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

		bool hasFile(const std::string& name) const override;
		std::string getFile(const std::string& name) const override;

	private:

		const std::string directory = "";
	};

	/**
	Customizable string preprocessor which follows ANSI-C preprocessor standard.
	<p>
	It can define all standard and operating system macros with special functions. Additionally, the following macros are always defined:
	| Macro name             | Macro definition                                                                                                                   |
	|------------------------|------------------------------------------------------------------------------------------------------------------------------------|
	| `_MACE_`             | Always defined                                                                                                                     |
	| `__INCLUDE_LEVEL__`    | Incremented by 1 when a file is included, and decremented when the included file ends. Identical to the GCC macro of the same name |
	| `__IF_SCOPE__`         | The depth of the current preprocessor conditional.  Shouldn't be used.                                                             |
	| `__CURRENT_IF_SCOPE__` | The level at which a conditional directive returned false. Shouldn't be used.                                                      |

	Even if they are not defined, you can never defined or undefine a standard macro as denoted by the ANSI-C standard. You also can't define or undefine keywords
	<p>
	Every directive in the ANSI-C standard is supported except for `#if`, `#elif`, and `#pragma`. `#warning` outputs a message to console. Additionally, `#version` is silenty ignored instead of being an error for GLSL support.
	<p>
	Each `Preprocessor` object stores a single string called the `input`.
	The only way to change the `input` is by cloning it via a special constructor.
	<p>
	`Preprocessor` can also 'fake' include files through the `IncludeString` class, allowing it to preprocess the same at every runtime invocation.
	<p>
	@internal The `OGL33Renderer` uses the `Preprocessor` class to handle shader files.

	@see Macro
	@see Include
	@todo Digraphs (see 6.4.6 of standard)
	@todo Variable arguments
	@todo Macro concat
	@todo `#if and `#elif` (funtion parseIfStatement exists and needs to get a definition)
	@todo `#pragma` (if STDC follows, do no macro replacement. if once follows, only include file once)
	*/
	class Preprocessor {
	public:
		/**
		Creates a `Preprocessor`
		@param input What to process.
		@param filename Name of the file that it is processing. This is used when an error happens. If not provided, "Unknown file" is used instead.
		*/
		Preprocessor(const std::string& input, const std::string& filename = "Unknown file");
		/**
		Clones another `Preprocessor` and gives it a new input. Macros and includes are all cloned.
		@param input New string that should be processed.
		@param clone Another `Preprocessor` whose features should be cloned.
		*/
		explicit Preprocessor(const std::string& input, const Preprocessor& clone);

		/**
		Processeses the `input` specified in the constructor according to the ANSI C standard. Macros are expanded, comments are stripped, and directives are executed.
		<p>
		Processing is done in O(N) time, where N is the amount of characters in the input. The entire process is done in 1 loop. This is accomplished via a complex finite state machine.
		@return A `std::vector` with the processed output. Each element is a token as specified by the standard.
		@see preprocess()
		@throws PreprocessorException If `input` is not valid ANSI C code.
		*/
		std::vector< std::string > preprocessTokens();
		/**
		Processeses the `input` specified in the constructor according to the ANSI C standard. Macros are expanded, comments are stripped, and directives are executed.
		<p>
		This is the same as using `preprocessTokens()` with every element combined.
		@return The processed input.
		@see preprocessTokens()
		@throws PreprocessorException If `input` is not valid ANSI C code.
		*/
		std::string preprocess();

		/**
		Adds an `Include` to this preprocessor. Any `Include`'s in this `Preprocessor` will be used when #include is encountered.
		<p>
		Because this function takes in a reference, the `Include` provided must remain owned by the program when `preprocess()` or `preprocessTokens()` is called.
		@param incl Reference to an `Include`
		@see preprocess()
		@see IncludeDirectory
		@see IncludeString
		*/
		void addInclude(const Include& incl);

		/**
		Retrieves all pointers to an `Include` added via `addInclude()` or `setIncludes()`
		@return A `std::vector` containing all `Includes` in this `Preprocessor`
		*/
		std::vector< const Include* > getIncludes();
		/**
		@copydoc Preprocessor::getIncludes()
		*/
		const std::vector< const Include* > getIncludes() const;
		/**
		Sets all of the `Includes` in this `Preprocessor`.
		@param includes The new `Includes`
		@see addInclude(Include&)
		*/
		void setIncludes(const std::vector< const Include* > includes);

		/**
		Retrieves what this `Preprocessor` is processing. Cannot be changed. Set by the constructor.
		@return A `const std::string` with the input. This will never change.
		@see Preprocessor(std::string, std::string)
		*/
		const std::string getInput() const;

		/**
		Gets the filename of the file this is processing. The filename is determined by the constructor, `#line`, or #include.
		<p>
		The `__FILE__` macro is equal to whatever this function returns.
		@return The name of the file being processed.
		*/
		std::string getFilename();
		/**
		@copydoc Preprocessor::getFilename()
		*/
		const std::string getFilename() const;
		/**
		Changes the filename. When `#line` is called with 2 arguments in the `input`, calls this function
		@param filename What to change the internal filename to. `__FILE__` will be updated to reflect the change.
		@see Preprocessor::getLine()
		*/
		void setFilename(const std::string& filename);

		/**
		Gets the current line number. The `__LINE__` macro's value is always equal to this function and `#line` can change the line number.
		<p>
		If the processing is done, this is equal to the amount of lines the input had.
		@return The current line that is being processed.
		*/
		unsigned int getLine();
		/**
		@copydoc Preprocessor::getLine()
		*/
		const unsigned int getLine() const;
		/**
		Sets the line number. This does not affect processing. This function is called when the `#line` directive is encountered in the `input`
		@param line The new line number
		@see getLine()
		*/
		void setLine(const unsigned int line);

		/**
		Defines a `Macro`
		<p>
		Called when #define is found in the `input`
		@param macro What to define.
		@see Macro
		@see undefineMacro(const std::string&)
		*/
		void defineMacro(const Macro& macro);
		/**
		Checks if a `Macro` with a certain name is defined. Called when `#ifdef` is encountered in the `input`
		@param name Name of the `Macro` to search for
		@return Whether it is defined.
		*/
		bool isMacroDefined(const std::string& name) const;
		/**
		Removes a `Macro` with the specified name. Called when #undef is encountered in the `input`
		<p>
		Because the standard requires #undef to silently do nothing if the macro does not exist, this function does the same.
		@param name Name of the `Macro` to undefine
		*/
		void undefineMacro(const std::string& name);
		/**
		Calculates the amount of `Macros` in this `Preprocessor`
		@return Size of the internal `Macro` array.
		*/
		Size macroNumber() const;

		/**
		Defines macros required by ANSI C and various compiler-specific macros. However, it will not define compiler-defining macros such as `__GCC__`.
		<p>
		Compiler-specific macros are from the following compilers:
		- GCC
		- VC++
		- Clang
		- Intel
		- Borland
		- Oracle
		<p>
		These function will only define macros that have values at the time that MACE is compiled. For example, if you compiler does not define `__STDC__`, this function will not define `__STDC__`.
		Additionally, only standard macros documented in the compiler's manual will be defined here. Custom macros that you define in the MACE compilation will not be defined by this function. For that, use the `defineMacro(Macro)` function.
		<p>
		Table of standard, non compiler specific macros:
		| Macro name        | Macro definition                                                                                                                                                                 |
		|-------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
		| `__FILE__`        | The current filename as specified by the constructor, #include, or,`#line` directive. If `#line` is called, this macro is defined even if `defineStandardMacros()` was not called.|
		| `__LINE__`        | The current line number in the file. Can be changed via `#line`. If `#line` is called, this macro is defined even if `defineStandardMacros()` was not called.                      |
		| `__DATE__`        | Equal to the date at which the `input` was processed with the format MNN DD YYYY. Only updates when `defineStandardMacros()` is called.                                          |
		| `__TIME__`        | Equal to the time at which the `input` was processed with the format HH:MM:SS. Only updates when `defineStandardMacros()` is called.                                             |
		| `__BASE_FILE__`   | The filename from which the processing starts. #include does not affect this macro, but #line does.                                                                             |
		| `__STDC__`        | Whether the preprocessor follows standard. The value of this macro is dependent on the value of __STDC__ when MACE was compiled.                                                 |
		| `__STDC_HOSTED__` | Whether this supports a hosted implementation. The value of this macro is equal to it's value when MACE was compiled. The standard requires this, so it is 1 most of the time.   |
		| `__cpluplus`      | What version of C++ is being used. This is equal to the version used to compile MACE.																				             |
		@see defineOSMacros()
		*/
		void defineStandardMacros();
		/**
		Defines macros that determine what OS you are on.
		The values of these macros are determined at compile time based on your compiler
		@see defineStandardMacros()
		*/
		void defineOSMacros();

		/**
		Finds a `Macro` with the given name.
		@param name What to look for
		@return A `Macro` with the specified name
		@throws PreprocessorException If the `Macro` with the name is not found.
		*/
		const Macro& getMacro(const std::string& name) const;
	private:
		const std::string input;

		unsigned int line = 0;

		std::string filename = "";

		std::vector< const Include* > includes;
		std::vector< Macro > macros;

		std::vector< std::string > parse();

		std::vector< std::string > executeDirective(bool& outputValue, const std::string& command, const std::string& params);

		int getMacroLocation(const std::string& name) const;

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