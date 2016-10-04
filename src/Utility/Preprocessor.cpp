#include <MACE/System/Utility/Preprocessor.h>
#include <MACE/System/Constants.h>
#include <iostream>
#include <ctype.h>
#include <ctime>
#include <cstring>
#include <algorithm>

/*
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define localtime(pointer,time) localtime_s(pointer,time)
#elif defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#define localtime(pointer,time) localtime_r(time,pointer)
#else
#error "You must be on a POSIX or Windows system in order to preprocess"
#endif
*/


//indirection is the only way to expand macros in other macros
#define NAME_STRINGIFY(name) "" #name
#define STRINGIFY(name) #name
//the strcmp checks if the macro is defined. if the name is different from it expanded, then it is a macro. doesnt work if a macro is defined as itself, but that shouldnt happen
#define PREDEFINE_MACRO(name) if(std::strcmp("" #name ,NAME_STRINGIFY(name))){setMacro( Macro( #name , STRINGIFY( name ) ));}

#ifndef STRINGIFY
#error blah
#endif

namespace mc {

	//todo:
	/*
	digraphs (see 6.4.6 of standard)

	__VA_ARGS__

	self-referntial macros up to 1 recurse

	concat of macros

	#if, !, &&, (), ||
	#elif

	#if defined () or # if defined name

	#include (and 15 nesting levels)

	#pragma (if STDC follows, do no macro replacement)


	MACROS (can't be undef:)
	__FILE__ - done
	__LINE__ - done
	__DATE__
	__TIME__
	__STDC__ - done
	__STDC_HOSTED__ - done
	__BASE_FILE__ - done
	__STDC_VERSION__ - sometimes defined
	__MACE__ - done
	__INCLUDE_LEVEL__
	__IF_SCOPE__ - done
	__CURRENT_IF_SCOPE__ - done,the scope at which a conditional returned false, shouldnt really be used


	BUGS:

	" \"hi \
	#	error this also doesnt foo test string literals" - doesnt output second line
	*/

	//these words can't be defined or undefined
	const std::string Preprocessor::reservedWords[] = {

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

	const std::vector< char > Preprocessor::punctuators1c = {
		',','\"','\'','{','}','[',']','~','.','|','&','+','-','*','/','=',';','!','%','>','<',':','?'
	};

	const std::vector< std::string > Preprocessor::punctuators2c = {
		">>","<<","++","--","+=","-=","*=","/=","&=","|=","%=","==","!=",">=","<=","&&","||","->","::","##"
	};

	Preprocessor::Preprocessor(const std::string& inputString, const std::string& filename) : input(inputString)
	{
		this->filename = filename;
	}

	Preprocessor::Preprocessor(const std::string & input, const Preprocessor & clone) : Preprocessor(input,clone.getFilename())
	{
		includeDirectories = clone.includeDirectories;
		macros = clone.macros;
	}

	std::vector<std::string> Preprocessor::preprocessTokens()
	{
		setMacro(Macro("__MACE__", "1"));

		setMacro(Macro("__BASE_FILE__", filename));
		setMacro(Macro("__FILE__", filename));

		//its std time boys
		std::time_t const now = std::time(0);

		struct tm time;
		//this macro is defined to either localtime_s or localtime_r
	//	localtime(&time, &now);



		PREDEFINE_MACRO(__STDC__);
		PREDEFINE_MACRO(__STDC_HOSTED__);
		PREDEFINE_MACRO(__STDC_MB_MIGHT_NEQ_WC__);
		PREDEFINE_MACRO(__STDC_ISO_10646__);
		PREDEFINE_MACRO(__STDC_VERSION__);
		PREDEFINE_MACRO(__STDC_IEC_559__);
		PREDEFINE_MACRO(__STDC_IEC_559_COMPLEX__);
		PREDEFINE_MACRO(__STDCPP_STRICT_POINTER_SAFETY__);

		//now, a giant list of predefined macros from various compilers, including gcc, clang, borland, oracle, digital mars, and vsc.
		//this list does not include compiler specific macros (denoted by a prefix, like __GCC_VERSION__)
		PREDEFINE_MACRO(__CHAR_UNSIGNED__);
		PREDEFINE_MACRO(__CHAR_UNSIGNED);
		PREDEFINE_MACRO(__WCHAR_UNSIGNED__);
		PREDEFINE_MACRO(__SIZE_TYPE__);
		PREDEFINE_MACRO(__PTRDIFF_TYPE__);
		PREDEFINE_MACRO(__WCHAR_TYPE__);
		PREDEFINE_MACRO(__WINT_TYPE__);
		PREDEFINE_MACRO(__INTMAX_TYPE__);
		PREDEFINE_MACRO(__UINTMAX_TYPE__);
		PREDEFINE_MACRO(__SIG_ATOMIC_TYPE__);
		PREDEFINE_MACRO(__INT8_TYPE__);
		PREDEFINE_MACRO(__INT16_TYPE__);
		PREDEFINE_MACRO(__INT32_TYPE__);
		PREDEFINE_MACRO(__INT64_TYPE__);
		PREDEFINE_MACRO(__UINT8_TYPE__);
		PREDEFINE_MACRO(__UINT16_TYPE__);
		PREDEFINE_MACRO(__UINT32_TYPE__);
		PREDEFINE_MACRO(__UINT64_TYPE__);
		PREDEFINE_MACRO(__INT_LEAST8_TYPE__);
		PREDEFINE_MACRO(__INT_LEAST16_TYPE__);
		PREDEFINE_MACRO(__INT_LEAST32_TYPE__);
		PREDEFINE_MACRO(__INT_LEAST64_TYPE__);
		PREDEFINE_MACRO(__UINT_LEAST8_TYPE__);
		PREDEFINE_MACRO(__UINT_LEAST16_TYPE__);
		PREDEFINE_MACRO(__UINT_LEAST32_TYPE__);
		PREDEFINE_MACRO(__UINT_LEAST64_TYPE__);
		PREDEFINE_MACRO(__INT_FAST8_TYPE__);
		PREDEFINE_MACRO(__INT_FAST16_TYPE__);
		PREDEFINE_MACRO(__INT_FAST32_TYPE__);
		PREDEFINE_MACRO(__INT_FAST64_TYPE__);
		PREDEFINE_MACRO(__UINT_FAST8_TYPE__);
		PREDEFINE_MACRO(__UINT_FAST16_TYPE__);
		PREDEFINE_MACRO(__UINT_FAST32_TYPE__);
		PREDEFINE_MACRO(__UINT_FAST64_TYPE__);
		PREDEFINE_MACRO(__INTPTR_TYPE__);
		PREDEFINE_MACRO(__UINTPTR_TYPE__);
		PREDEFINE_MACRO(__STDCPP_THREADS__);
		PREDEFINE_MACRO(__cplusplus);
		PREDEFINE_MACRO(__CHAR_BIT__);
		PREDEFINE_MACRO(__INT_SHORT__);
		PREDEFINE_MACRO(__SCHAR_MAX__);
		PREDEFINE_MACRO(__SHRT_MAX__);
		PREDEFINE_MACRO(__INT_MAX__);
		PREDEFINE_MACRO(__LONG_MAX__);
		PREDEFINE_MACRO(__LONG_LONG_MAX__);
		PREDEFINE_MACRO(__INTMAX_MAX__);
		PREDEFINE_MACRO(_DEBUG);
		PREDEFINE_MACRO(_INTEGRAL_MAX_BITS);
		PREDEFINE_MACRO(__LP64__);
		PREDEFINE_MACRO(_LP64);
		PREDEFINE_MACRO(__OBJC__);
		PREDEFINE_MACRO(__ASSEMBLER__);
		PREDEFINE_MACRO(__DEPRECATED);
		PREDEFINE_MACRO(__USER_LABEL_PREFIX__);
		PREDEFINE_MACRO(__REGISTER_PREFIX__);
		PREDEFINE_MACRO(__ELF__);
		PREDEFINE_MACRO(__OPTIMIZE__);
		PREDEFINE_MACRO(__OPTIMIZE_SIZE__);
		PREDEFINE_MACRO(__NO_INLINE__);
		PREDEFINE_MACRO(__NO_MATH_ERRNO__);
		PREDEFINE_MACRO(__FP_FAST_FMA);
		PREDEFINE_MACRO(__FP_FAST_FMAF);
		PREDEFINE_MACRO(__FP_FAST_FMAL);
		PREDEFINE_MACRO(__USING_SJLJ_EXCEPTIONS__);
		PREDEFINE_MACRO(__EXCEPTIONS);
		PREDEFINE_MACRO(__FLOAT_WORD_ORDER__);
		PREDEFINE_MACRO(__SCHAR_MAX__);
		PREDEFINE_MACRO(__WCHAR_MAX__);
		PREDEFINE_MACRO(__SHRT_MAX__);
		PREDEFINE_MACRO(__INT_MAX__);
		PREDEFINE_MACRO(__LONG_MAX__);
		PREDEFINE_MACRO(__LONG_LONG_MAX__);
		PREDEFINE_MACRO(__WINT_MAX__);
		PREDEFINE_MACRO(__SIZE_MAX__);
		PREDEFINE_MACRO(__PTRDIFF_MAX__);
		PREDEFINE_MACRO(__INTMAX_MAX__);
		PREDEFINE_MACRO(__UINTMAX_MAX__);
		PREDEFINE_MACRO(__SIG_ATOMIC_MAX__);
		PREDEFINE_MACRO(__INT8_MAX__);
		PREDEFINE_MACRO(__INT16_MAX__);
		PREDEFINE_MACRO(__INT32_MAX__);
		PREDEFINE_MACRO(__INT64_MAX__);
		PREDEFINE_MACRO(__UINT8_MAX__);
		PREDEFINE_MACRO(__UINT16_MAX__);
		PREDEFINE_MACRO(__UINT32_MAX__);
		PREDEFINE_MACRO(__UINT64_MAX__);
		PREDEFINE_MACRO(__INT_LEAST8_MAX__);
		PREDEFINE_MACRO(__INT_LEAST16_MAX__);
		PREDEFINE_MACRO(__INT_LEAST32_MAX__);
		PREDEFINE_MACRO(__INT_LEAST64_MAX__);
		PREDEFINE_MACRO(__UINT_LEAST8_MAX__);
		PREDEFINE_MACRO(__UINT_LEAST16_MAX__);
		PREDEFINE_MACRO(__UINT_LEAST32_MAX__);
		PREDEFINE_MACRO(__UINT_LEAST64_MAX__);
		PREDEFINE_MACRO(__INT_FAST8_MAX__);
		PREDEFINE_MACRO(__INT_FAST16_MAX__);
		PREDEFINE_MACRO(__INT_FAST32_MAX__);
		PREDEFINE_MACRO(__INT_FAST64_MAX__);
		PREDEFINE_MACRO(__UINT_FAST8_MAX__);
		PREDEFINE_MACRO(__UINT_FAST16_MAX__);
		PREDEFINE_MACRO(__UINT_FAST32_MAX__);
		PREDEFINE_MACRO(__UINT_FAST64_MAX__);
		PREDEFINE_MACRO(__INTPTR_MAX__);
		PREDEFINE_MACRO(__UINTPTR_MAX__);
		PREDEFINE_MACRO(__WCHAR_MIN__);
		PREDEFINE_MACRO(__WINT_MIN__);
		PREDEFINE_MACRO(__SIG_ATOMIC_MIN__);
		PREDEFINE_MACRO(__INT8_C);
		PREDEFINE_MACRO(__INT16_C);
		PREDEFINE_MACRO(__INT32_C);
		PREDEFINE_MACRO(__INT64_C);
		PREDEFINE_MACRO(__UINT8_C);
		PREDEFINE_MACRO(__UINT16_C);
		PREDEFINE_MACRO(__UINT32_C);
		PREDEFINE_MACRO(__UINT64_C);
		PREDEFINE_MACRO(__INTMAX_C);
		PREDEFINE_MACRO(__UINTMAX_C);
		PREDEFINE_MACRO(__SSP__);
		PREDEFINE_MACRO(__SSP_ALL__);
		PREDEFINE_MACRO(__SSP_STRONG__);
		PREDEFINE_MACRO(__SSP_EXPLICIT__);
		PREDEFINE_MACRO(__SANITIZE_ADDRESS__);
		PREDEFINE_MACRO(__SANITIZE_THREAD__);
		PREDEFINE_MACRO(__NO_MATH_ERRNO__);
		PREDEFINE_MACRO(__FP_FAST_FMA);
		PREDEFINE_MACRO(__FP_FAST_FMAF);
		PREDEFINE_MACRO(__FP_FAST_FMAL);
		PREDEFINE_MACRO(__NEXT_RUNTIME__);
		PREDEFINE_MACRO(__STRICT_ANSI__);
		PREDEFINE_MACRO(__BYTE_ORDER__);
		PREDEFINE_MACRO(__ORDER_LITTLE_ENDIAN__);
		PREDEFINE_MACRO(__ORDER_BIG_ENDIAN__);
		PREDEFINE_MACRO(__ORDER_PDP_ENDIAN__);
		PREDEFINE_MACRO(__SCHAR_WIDTH__);
		PREDEFINE_MACRO(__SHRT_WIDTH__);
		PREDEFINE_MACRO(__INT_WIDTH__);
		PREDEFINE_MACRO(__LONG_WIDTH__);
		PREDEFINE_MACRO(__LONG_LONG_WIDTH__);
		PREDEFINE_MACRO(__PTRDIFF_WIDTH__);
		PREDEFINE_MACRO(__SIG_ATOMIC_WIDTH__);
		PREDEFINE_MACRO(__SIZE_WIDTH__);
		PREDEFINE_MACRO(__WCHAR_WIDTH__);
		PREDEFINE_MACRO(__WINT_WIDTH__);
		PREDEFINE_MACRO(__INT_LEAST8_WIDTH__);
		PREDEFINE_MACRO(__INT_LEAST16_WIDTH__);
		PREDEFINE_MACRO(__INT_LEAST32_WIDTH__);
		PREDEFINE_MACRO(__INT_LEAST64_WIDTH__);
		PREDEFINE_MACRO(__INT_FAST8_WIDTH__);
		PREDEFINE_MACRO(__INT_FAST16_WIDTH__);
		PREDEFINE_MACRO(__INT_FAST32_WIDTH__);
		PREDEFINE_MACRO(__INT_FAST64_WIDTH__);
		PREDEFINE_MACRO(__INTPTR_WIDTH__);
		PREDEFINE_MACRO(__INTMAX_WIDTH__);
		PREDEFINE_MACRO(__SIZEOF_INT__);
		PREDEFINE_MACRO(__SIZEOF_LONG__);
		PREDEFINE_MACRO(__SIZEOF_LONG_LONG__);
		PREDEFINE_MACRO(__SIZEOF_SHORT__);
		PREDEFINE_MACRO(__SIZEOF_POINTER__);
		PREDEFINE_MACRO(__SIZEOF_FLOAT__);
		PREDEFINE_MACRO(__SIZEOF_DOUBLE__);
		PREDEFINE_MACRO(__SIZEOF_LONG_DOUBLE__);
		PREDEFINE_MACRO(__SIZEOF_SIZE_T__);
		PREDEFINE_MACRO(__SIZEOF_WCHAR_T__);
		PREDEFINE_MACRO(__SIZEOF_WINT_T__);
		PREDEFINE_MACRO(__SIZEOF_PTRDIFF_T__);
		PREDEFINE_MACRO(__ATOM__);
		PREDEFINE_MACRO(__AVX__);
		PREDEFINE_MACRO(__AVX2__);
		PREDEFINE_MACRO(_CONTROL_FLOW_GUARD);
		PREDEFINE_MACRO(__cplusplus_cli);
		PREDEFINE_MACRO(__cplusplus_winrt);
		PREDEFINE_MACRO(_CPPRTTI);
		PREDEFINE_MACRO(_CPPUNWIND);
		PREDEFINE_MACRO(_DLL);
		PREDEFINE_MACRO(__INTELLISENSE__);
		PREDEFINE_MACRO(_ISO_VOLATILE);
		PREDEFINE_MACRO(_KERNEL_MODE);
		PREDEFINE_MACRO(_WINRT_DLL);
		PREDEFINE_MACRO(_WCHAR_T_DEFINED);
		PREDEFINE_MACRO(_VC_NODEFAULTLIB);
		PREDEFINE_MACRO(_PREFAST_);
		PREDEFINE_MACRO(_OPENMP);
		PREDEFINE_MACRO(_NATIVE_WCHAR_T_DEFINED);
		PREDEFINE_MACRO(_MT);
		PREDEFINE_MACRO(_MANAGED);
		PREDEFINE_MACRO(_WCTYPE_T_DEFINED);
		PREDEFINE_MACRO(_PLACEMENT_DELETE);
		PREDEFINE_MACRO(_PGO_INSTRUMENT);
		PREDEFINE_MACRO(_BOOL);
		PREDEFINE_MACRO(__WINT_TYPE__);
		PREDEFINE_MACRO(__SSSE3__);
		PREDEFINE_MACRO(__SSE4_2__);
		PREDEFINE_MACRO(__SSE4_1__);
		PREDEFINE_MACRO(__SSE3__);
		PREDEFINE_MACRO(__SSE2__);
		PREDEFINE_MACRO(__SSE__);
		PREDEFINE_MACRO(__SIZE_TYPE__);
		PREDEFINE_MACRO(__SIGNED_CHARS__);
		PREDEFINE_MACRO(__QMSPP_);
		PREDEFINE_MACRO(__PIC__);
		PREDEFINE_MACRO(__pic__);
		PREDEFINE_MACRO(__NO_MATH_INLINES);
		PREDEFINE_MACRO(__NO_STRING_INLINES);
		PREDEFINE_MACRO(__MMX__);
		PREDEFINE_MACRO(__MIC__);
		PREDEFINE_MACRO(__LONG_DOUBLE_64__);
		PREDEFINE_MACRO(__LONG_DOUBLE_SIZE__);
		PREDEFINE_MACRO(i386);
		PREDEFINE_MACRO(__i386__);
		PREDEFINE_MACRO(__i386);
		PREDEFINE_MACRO(__HONOR_STD);
		PREDEFINE_MACRO(__GFX__);
		PREDEFINE_MACRO(__extension__);
		PREDEFINE_MACRO(__EDG__);
		PREDEFINE_MACRO(__EDG_VERSION__);
		PREDEFINE_MACRO(__DYNAMIC__);
		PREDEFINE_MACRO(__DEPRECATED);
		PREDEFINE_MACRO(__cilk);
		PREDEFINE_MACRO(__ARRAY_OPERATORS);
		PREDEFINE_MACRO(__AVX__);
		PREDEFINE_MACRO(__AVX2__);
		PREDEFINE_MACRO(__AVX512BW__);
		PREDEFINE_MACRO(__AVX512CD__);
		PREDEFINE_MACRO(__AVX512DQ__);
		PREDEFINE_MACRO(__AVX512ER__);
		PREDEFINE_MACRO(__AVX512F__);
		PREDEFINE_MACRO(__AVX512PF__);
		PREDEFINE_MACRO(__AVX512VL__);
		PREDEFINE_MACRO(__ARRAYNEW);
		PREDEFINE_MACRO(__spar);
		PREDEFINE_MACRO(sparc);
		PREDEFINE_MACRO(__sparcv);
		PREDEFINE_MACRO(__SVR4);
		PREDEFINE_MACRO(_ENABLE_ARRAYNEW);
		PREDEFINE_MACRO(_PUSHPOP_SUPPORTED);
		PREDEFINE_MACRO(__INTSIZE);
		PREDEFINE_MACRO(__DEFALIGN);
		PREDEFINE_MACRO(__INLINE_8087);
		PREDEFINE_MACRO(__I86__);
		PREDEFINE_MACRO(__NT__);
		PREDEFINE_MACRO(__SC__);
		PREDEFINE_MACRO(__ZTC__);
		PREDEFINE_MACRO(DEBUG);
		PREDEFINE_MACRO(__FPCE__IEEE__);
		PREDEFINE_MACRO(__FPCE__);
		PREDEFINE_MACRO(__VCM__);
		PREDEFINE_MACRO(__SMALL__);
		PREDEFINE_MACRO(__MEDIUM__);
		PREDEFINE_MACRO(__COMPACT__);
		PREDEFINE_MACRO(__LARGE__);

		return parse(input);
	}

	std::string Preprocessor::preprocess()
	{
	

		std::string out = "";
		std::vector < std::string > tokens = preprocessTokens();
		
		for (unsigned int i = 0; i < tokens.size(); i++) {
			out += tokens[i];
		}
		return out;
	}

	std::vector < std::string > Preprocessor::parse(const std::string & input)
	{
		std::string command = "", params = "",currentToken="";

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
		enum STATE { PROBING, NEWLINE, FINDING_COMMAND_START, COMMAND_NAME, PARAMETERS, STRING_LITERAL, SINGLELINE_COMMENT, MULTILINE_COMMENT,PARENTHESIS};

		enum STATE state = PROBING;

		/*
		If set to true, the current character will be added to final output. This is used when an #if/#ifdef/#ifndef/#elif is called with a false condition.
		This is used because a false if condition will last multiple lines. The only other thing that doesnt output for multiple lines is a multiline comment, however it only needs to check for multiple characters.

		Instead of using outputvalue, continue should be used instead.
		*/
		bool outputValue = true;

		//used to accurately parse function(function(arg))
		int parenScope = 0;

		for (Index iter = 0; iter <= input.length(); ++iter) {
			char value;
			//we add a newline at the end, because at the end of a line we do special calculations, and one liners need to do them as well
			if (iter == input.length()) {
				value = '\n';
			}
			else {
				value = (input[iter]);
			}

			//this if statement does the backslash-newline, and converts it to a space by checking the current character is \ and the next one is \n
			if (value=='\\'&&(iter < input.length()-1 && input[iter + 1] == '\n')) {
				++iter;
				++line;
				setMacro(Macro("__LINE__", std::to_string(line)));
				continue;
			}
			//if its not a backslash-newline, we check if its just a newline, obviously multiline comments ignore newlines
			else if ((value == '\n')) {
				if (state != MULTILINE_COMMENT){
					state = NEWLINE;
				}
				++line;
				setMacro(Macro("__LINE__",std::to_string(line)));
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
				else if (value == '\0') {
					value = ' '; 
				}
				else if (value == '(') {
					++parenScope;
					state = PARENTHESIS;
				}

				break;

			case FINDING_COMMAND_START:
				if (!isspace(value))state = COMMAND_NAME;

			case COMMAND_NAME:
				if (isspace(value)) {
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

				if (value == '"' || value == '\'') {
					state = PROBING;
				}

				if(outputValue)currentToken += value;
				continue;

			case SINGLELINE_COMMENT:
				continue;

			case MULTILINE_COMMENT:
				if (value == '/'&&input[iter - 1] == '*') {
					//we increment iter to account for the next / character
					state = PROBING;
				}
				continue;

			case PARENTHESIS:
				if(outputValue)currentToken += value;
				if (value == ')') {
					--parenScope;
					if (parenScope == 0) {
						state = PROBING;
					}
				}
				else if (value == '(') {
					++parenScope;
				}
				continue;
		
			case NEWLINE:
				if (!command.empty()) {
					/*outputValue is passed by reference so it can be changed
					*/
					std::vector < std::string > directiveTokens = executeDirective(outputValue,command,params);

					tokens.insert(tokens.end(),directiveTokens.begin(),directiveTokens.end());
				}
				command = "";
				params = "";
				state = PROBING;
				break;
			}

			if (outputValue) {
				if (!currentToken.empty() && (isNewToken(currentToken.back(),value))) {
					tokens.push_back(expandMacro(currentToken));
					currentToken = "";
				}

				currentToken += value;
			}
		}

		


		const int macroLocation = getMacroLocation("__IF_SCOPE__");
		if (macroLocation >= 0) {
		//	if (std::stoi(getMacro("__IF_SCOPE__")) > 0)throw PreprocessorException(getLocation() + ": #if directive is missing an #endif");
		}

		return tokens;
	}

	std::vector < std::string > Preprocessor::executeDirective(bool& outputValue, const std::string & command, const std::string & params)
	{
		//only if we output
		if (outputValue) {
			if (command == "error") {
				if (params.empty())throw PreprocessorException(getLocation() + ": #error must be called with a message");
				throw PreprocessorException(getLocation() + ": " + params);
			}
			else if (command == "warning") {
				if (params.empty())throw PreprocessorException(getLocation() + ": #warning must be called with a message");
				std::cout << getLocation() << ": " << params << std::endl;
			}
			else if (command == "line") {
				if (params.empty())throw PreprocessorException(getLocation() + ": #line must be called with a line number");

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

				setMacro(Macro("__LINE__", std::to_string(line)));
				setMacro(Macro("__FILE__", (filename)));
			}
			else if (command == "define") {
				if (params.empty())throw PreprocessorException(getLocation() + ": #define must be called with a name and definition");

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
					MACRO_NAME,PARAMETERS
				};

				enum STATE parseState = MACRO_NAME;

				std::string currentParameter = "";

				//first we get the line number
				for (iterator = 0; iterator < params.length(); ++iterator) {
					char value = params[iterator];

					if (parseState == MACRO_NAME) {
						if (value == '(') {
							parseState = PARAMETERS;
							continue;
						}
						else if (isspace(value)) {
							++iterator;//lets also increment the iterator so the space doesnt get added to the macroname
							break;//the second word, lets go to the second loop
						}

						macroName += value;

					}
					else if (parseState == PARAMETERS) {
						//we dont want spaces in our parameters
						if (isspace(value)) {
							continue;
						}
						else if(value==')'){
							++iterator;
							macroParameters.push_back(currentParameter);
							break;
						}
						else if (value == ',') {
							macroParameters.push_back(currentParameter);
							currentParameter = "";
							continue;
						}
						else {
							currentParameter += value;
						}
					}

				}

				//remaining characters are added to the filename
				for (iterator; iterator < params.length(); ++iterator) {
					macroDefinition += params[iterator];
				}

				if (macroDefinition == "")throw PreprocessorException(getLocation() + ": a macro must have a definition");

				defineMacro(Macro(macroName, macroDefinition,macroParameters,""));
			}
			else if (command == "undef") {
				if (params.empty())throw PreprocessorException(getLocation() + ": #undef must be called with a macro name");

				Index iterator;
				std::string macroName = "";

				//first we get the line number
				for (iterator = 0; iterator < params.length(); ++iterator) {
					if (isspace(params[iterator])) {
						++iterator;//lets also increment the iterator so the space doesnt get added to the filename
						break;//the second word, lets go to the second loop
					}
					macroName += params[iterator];
				}

				undefineMacro(params);
			}
		}

		if (command == "if") {
			if (params.empty())throw PreprocessorException(getLocation() + ": #if must be called with an argument");

			calculateIfScope(outputValue,parseIfStatement(params));
		}
		else if (command == "elif") {
			if (params.empty())throw PreprocessorException(getLocation() + ": #elif must be called with an argument");

			calculateIfScope(outputValue, parseIfStatement(params));
		}
		else if (command == "ifdef") {
			if (params.empty())throw PreprocessorException(getLocation() + ": #ifdef must be called with a macro name");

			Index iterator;
			std::string macroName = "";

			//first we get the line number
			for (iterator = 0; iterator < params.length(); ++iterator) {
				if (isspace(params[iterator])) {
					++iterator;//lets also increment the iterator so the space doesnt get added to the filename
					break;//the second word, lets go to the second loop
				}
				macroName += params[iterator];
			}

			calculateIfScope(outputValue, isMacroDefined(macroName));
		}
		else if (command == "ifndef") {
			if (params.empty())throw PreprocessorException(getLocation() + ": #ifdef must be called with a macro name");

			Index iterator;
			std::string macroName = "";

			//first we get the line number
			for (iterator = 0; iterator < params.length(); ++iterator) {
				if (isspace(params[iterator])) {
					++iterator;//lets also increment the iterator so the space doesnt get added to the filename
					break;//the second word, lets go to the second loop
				}
				macroName += params[iterator];
			}

			calculateIfScope(outputValue, !isMacroDefined(macroName));
		}
		else if (command == "else") {
			int ifScope = getMacroLocation("__IF_SCOPE__");
			if (ifScope < 0||std::stoi(macros[ifScope].definition)<=0) {
				throw PreprocessorException(getLocation() + ": #else is missing an if directive");
			}

			int currentIfScope = getMacroLocation("__CURRENT_IF_SCOPE__");
			if (currentIfScope < 0) {
				throw PreprocessorException(getLocation() + ": #else is missing an if directive");
			}


			if (outputValue||macros[ifScope].definition == macros[currentIfScope].definition) {
				outputValue = !outputValue;
				if (outputValue) {
					macros[currentIfScope].definition = "0";
				}
				else {
					macros[currentIfScope].definition = macros[ifScope].definition;
				}
			}
		}
		else if (command == "endif") {
			int ifScope = getMacroLocation("__IF_SCOPE__");
			if (ifScope < 0 || std::stoi(macros[ifScope].definition) <= 0) {
				throw PreprocessorException(getLocation()+": #endif is missing an if directive");
			}

			int currentIfScope = getMacroLocation("__CURRENT_IF_SCOPE__");
			if (currentIfScope < 0) {
				throw PreprocessorException(getLocation() + ": #endif is missing an if directive");
			}


			if (!outputValue&&macros[ifScope].definition==macros[currentIfScope].definition) {
				outputValue = true;
				setMacro(Macro("__CURRENT_IF_SCOPE__","0"));
			}

			macros[ifScope].definition = std::to_string(std::stoi(macros[ifScope].definition) - 1);

		}
		

		return std::vector < std::string >();
	}

	int Preprocessor::getMacroLocation(const std::string & name) const
	{
		for (Index iterator = 0; iterator < macros.size(); ++iterator) {
			if (macros[iterator].name == name) {
				return iterator;
			}
		}
		return -1;
	}

	bool Preprocessor::isNewToken(const char lastValue, const char value) const
	{

		//This is quite a complex if statement, but it checks to make sure that == doesnt become = = and a=b becomes a = b
		bool out = (isspace(value)
			|| ((std::find(punctuators1c.begin(), punctuators1c.end(), lastValue) != punctuators1c.end()
				|| (!isNonOperator(lastValue) && isNonOperator(value))
				|| (isNonOperator(lastValue) && !isNonOperator(value))
				)
				&& (std::find(punctuators2c.begin(), punctuators2c.end(), std::string{ lastValue,value }) == punctuators2c.end())
				));
		return out;
	}

	bool Preprocessor::isNonOperator(const char value) const
	{
		return std::find(punctuators1c.begin(), punctuators1c.end(), value) == punctuators1c.end();
	}

	std::string Preprocessor::expandMacro(const std::string input) const
	{
		if(!input.empty()||(input.length()==1&&(isspace(input[0])||iscntrl(input[0])||isblank(input[0])))){

			std::string out = input;

			bool stringify = false;

			//if we get input like foo(), we need to check if foo exists, not foo()
			Macro token = parseMacroName(input);

			//remove whitespace to check for macro. a line like int a = MACRO wont work without this line, as it would be tokenized to be (a),( =), and ( MACRO)
			token.name.erase(std::remove_if(token.name.begin(), token.name.end(), isspace), token.name.end());

			if (token.name[0] == '#') {
				//get rid of the # so we can find the macro name
				token.name = token.name.substr(1);

				stringify = true;
			}

			const int macroLocation = getMacroLocation(token.name);
			if (macroLocation >= 0) {

				//the space is to prevent the execution of directives
				Preprocessor p = Preprocessor(' '+macros[macroLocation].definition, *this);

				for (Index i = 0; i < token.parameters.size(); ++i) {

					Preprocessor argumentProcessor = Preprocessor(token.parameters[i],*this);

					p.defineMacro(Macro(macros[macroLocation].parameters[i],argumentProcessor.preprocess()));
				}

				//the substr one is to get rid of the space we added in the previous line
				std::string replacement = p.preprocess().substr(1);

				if (stringify) { 
					//get rid of the # for stringication
					out.erase(std::remove_if(out.begin(), out.end(), [](char c) {
						return c == '#';
					}), out.end());

					replacement = '\"' + replacement + '\"'; 
				}

				//these lines are to get rid of the parameters from the final output
				const int parametersLocation = out.find("(");
				if (parametersLocation != std::string::npos) {
					out = out.substr(0, parametersLocation);
				}

				std::size_t index;

				//we want to preserve the whitespace in the final output, so this algorithim replaces the non-whitespace with the macro expansion
				while ((index = out.find(token.name)) != std::string::npos){
					out.replace(index, token.name.size(), replacement);
				}
			}//macroLocation>0
			else if (!token.parameters.empty()) {

				//if its a function, we expand the parameters

				std::cout << token.parameterString << " : ";


				//this will expand macros in the parameters
				Preprocessor outProcessor = Preprocessor(token.parameterString,*this);

				for (Index i = 0; i < token.parameters.size(); i++) {
					Preprocessor argumentProcessor = Preprocessor(token.parameters[i], *this);

					outProcessor.setMacro(Macro(token.parameters[i], argumentProcessor.preprocess()));
				}

				std::string result = outProcessor.preprocess();

				out = token.name + '(' + result + ')';

			}//!token.parameters.empty()

			return out;

		}//!input.empty

		return input;
	}

	int Preprocessor::getIfScopeLocation()
	{
		int ifScope = getMacroLocation("__IF_SCOPE__");
		if (ifScope < 0) {
			ifScope = macros.size();
			setMacro(Macro("__IF_SCOPE__", "0"));
		}

		return ifScope;
	}

	int Preprocessor::getCurrentIfScopeLocation()
	{

		int currentIfScope = getMacroLocation("__CURRENT_IF_SCOPE__");
		if (currentIfScope < 0) {
			currentIfScope = macros.size();
			setMacro(Macro("__CURRENT_IF_SCOPE__", "0"));
		}

		return currentIfScope;
	}

	void Preprocessor::calculateIfScope(bool& outputValue, const bool statementPassed)
	{
		int ifScope = getIfScopeLocation();
		int currentIfScope = getCurrentIfScopeLocation();

		macros[ifScope].definition = std::to_string(std::stoi(macros[ifScope].definition) + 1);

		if (!statementPassed&&std::stoi(macros[currentIfScope].definition) == 0) {
			macros[currentIfScope].definition = macros[ifScope].definition;
			outputValue = false;
		}
	}

	bool Preprocessor::parseIfStatement(const std::string statement)
	{
		return false;
	}

	Preprocessor::Macro Preprocessor::parseMacroName(const std::string & name) const
	{

		std::string macroName;
		std::string definition = "";
		std::vector < std::string > params = std::vector< std::string >();
		std::string parameterString = "";

		Index iter;

		int functionScope = 0;


		for (iter = 0; iter < name.length(); ++iter) {
			char value = name[iter];
			if (value == '(') {

				//we want to ignore the first parenthsis
				++iter;

				//continue the loop and parse the arguments
				std::string currentParam = "";
				//each time a ( is encountered, functionScope is incremented. This is used to make sure all parameter functions are parsed correctly

				for (iter; iter < name.length(); ++iter) {
					char value = name[iter];

					if (value == '(') {
						++functionScope;
					}
					//function scope needs to be equal to 1, as that means there has been one set of parenthesis. functionScope==0 means its not a function
					else if (value == ','&&functionScope == 0) {
						params.push_back(currentParam);
						currentParam = "";
					}
					else if (value == ')') {
						if (functionScope == 0) {
							++iter;
							params.push_back(currentParam);
							break;
						}
						else {
							currentParam += value;
						}

						--functionScope;
					}
					else if (!isspace(value)) {
						currentParam += value;
					}

					parameterString += value;
				}

				break;
			}else if (!isspace(value)) {
				macroName += value;
			}
		}

		if (functionScope < 0) {
			throw PreprocessorException(getLocation() + ": Missing ( in function name");
		}
		else if (functionScope > 0) {
			throw PreprocessorException(getLocation() + ": Missing ) in function name");
		}

		for (iter; iter < name.length(); ++iter) {
			definition += name[iter];
		}

		//the substr of parameterString is to remove the ( at the beginning.
		return Preprocessor::Macro(macroName,definition,params,parameterString);
	}

	void Preprocessor::setMacro(const Macro& m)
	{
		for (unsigned int iterator = 0; iterator < macros.size(); ++iterator) {
			if (macros[iterator].name == m.name) {
				macros[iterator].definition = m.definition;
				macros[iterator].parameters = m.parameters;
				return;
			}
		}
		macros.push_back(m);
	}


	void Preprocessor::defineMacro(const Macro& m)
	{
		for (unsigned int iterator = 0; iterator < sizeof(reservedWords) / sizeof(*reservedWords); ++iterator) {
			if (reservedWords[iterator] == m.name)throw PreprocessorException(getLocation()+": can\'t define a reserved word");
		}
		setMacro(m);
	}

	bool Preprocessor::isMacroDefined(const std::string & name) const
	{
		for (Index iterator = 0; iterator < macros.size(); ++iterator) {
			if (macros[iterator].name == name) {
				return true;
			}
		}
		return false;
	}

	void Preprocessor::undefineMacro(const std::string & name)
	{
		Index iterator = 0;

		for (iterator = 0; iterator < sizeof(reservedWords) / sizeof(*reservedWords); ++iterator) {
			if (reservedWords[iterator] == name)throw PreprocessorException(getLocation() + ": can\'t undefine a reserved word");
		}
		for (iterator = 0; iterator < macros.size(); ++iterator) {
			if (macros[iterator].name == name) {
				macros.erase(macros.begin()+iterator);
				return;
			}
		}

		//do nothing if its not found
	}

	const Preprocessor::Macro & Preprocessor::getMacro(const std::string & name) const
	{
		for (unsigned int iterator = 0; iterator < macros.size(); ++iterator) {
			if (macros[iterator].name == name) {
				return macros[iterator];
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

	//d is part of std? how lewd can we get
	Preprocessor::Macro::Macro(std::string n, std::string d, std::vector<std::string> p,std::string ps) : name(n),definition(d), parameters(p), parameterString(ps)
	{
	}


	Preprocessor::Macro::Macro(std::string n, std::string d) : Macro(n,d,std::vector< std::string >(),"")
	{
	}

}