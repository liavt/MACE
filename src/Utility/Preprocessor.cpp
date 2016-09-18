#include <MACE/System/Utility/Preprocessor.h>
#include <MACE/System/Constants.h>
#include <iostream>
#include <ctype.h>
#include <ctime>
#include <algorithm>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define localtime(pointer,time) localtime_s(pointer,time)
#elif defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#define localtime(pointer,time) localtime_r(time,pointer)
#else
#error "You must be on a POSIX or Windows system in order to preprocess"
#endif

//indirection is the only way to expand macros in other macros
#define NAME_STRINGIFY( name ) "" #name
#define STRINGIFY( name ) #name
//the strcmp checks if the macro is defined. if the name is different from it expanded, then it is a macro. doesnt work if a macro is defined as itself, but that shouldnt happen
#define PREDEFINE_MACRO(name) if(std::strcmp("" #name ,NAME_STRINGIFY(name))){std::cout<< #name <<std::endl;setMacro( #name , STRINGIFY( name ));}

namespace mc {

	//todo:
	/*
	digraphs (see 6.4.6 of standard)

	functional macros and __VA_ARGS__

	self-referntial macros up to 1 recurse

	stringification

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
	__CHAR_UNSIGNED__ - sometimes defined, could be __CHAR_UNSIGNED
	__WCHAR_UNSIGNED__ - sometimes defined
	__SIZE_TYPE__ - sometimes defined
	__PTRDIFF_TYPE__ - sometimes defined
	__WCHAR_TYPE__ - sometimes defined
	__INTMAX_TYPE__ - sometimes defined
	__UINTMAX_TYPE__ - sometimes defined
	__cplusplus - sometimes defined
	__STDC_MB_MIGHT_NEQ_WC__ - sometimes defined, if multibyte might give a character a different value
	__STDC_ISO_10646__ - sometimes defined, specifies date of unicode standard
	__STDCPP_STRICT_POINTER_SAFETY__ - sometimes defined, if has strict pointer safety
	__STDCPP_THREADS__ - sometimes defined, if program cna have more than 1 thread
	__CHAR_BIT__-  sometimes defined
	__INT_SHORT__-  sometimes defined
	__SCHAR_MAX__- sometimes defined
	__SHRT_MAX__-  sometimes defined
	__INT_MAX__-  sometimes defined
	__LONG_MAX__-  sometimes defined
	__LONG_LONG_MAX__-  sometimes defined
	__INTMAX_MAX__ - sometimes defined
	_DEBUG - sometimes defined
	_INTEGRAL_MAX_BITS - sometimes defined
	__LP64__ - sometimes defined,could be _LP64

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
		"__TIME__","__DATE__","__STDC__","__STDC_HOSTED__","__STDC_VERSION__"
	};

	const std::vector< char > Preprocessor::punctuators1c = {
		',','\"','\'','{','}','[',']','(',')','~','.','|','&','+','-','*','/','=',';','!','%','>','<',':','?'
	};

	const std::vector< std::string > Preprocessor::punctuators2c = {
		">>","<<","++","--","+=","-=","*=","/=","&=","|=","%=","==","!=",">=","<=","&&","||","->","::"
	};

	const std::vector< std::string > Preprocessor::punctuators3c = {
		">>=","<<="
	};

	Preprocessor::Preprocessor(const std::string& inputString, const std::string& filename) : input(inputString)
	{
		this->filename = filename;
	}

	std::string Preprocessor::preprocess()
	{
		setMacro("__MACE__","1");

		setMacro("__BASE_FILE__",filename);
		setMacro("__FILE__",filename);

		//its std time boys
		std::time_t const now = std::time(0);

		struct tm time;
		//this macro is defined to either localtime_s or localtime_r
		localtime(&time, &now);



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

		std::string out = "";
		std::vector < std::string > tokens = parse(input);
		
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

		PROBING starts first. If it finds a # character, it goes to FINDING_COMMAND_START. If it finds a " or ', it goes to STRING_LITERAL. If it finds a //, it goes to SINGLELINE_COMMENT

		FINDING_COMMAND_START looks for the first non-whitespace character, and when it does, goes to COMMAND_NAME
		COMMAND_NAME adds every character to command until it encounters a whitespace character, where it goes to PARAMETERS
		PARAMETERS adds every character to params

		STRING_LITERAL does nothing until it finds " or ', where it goes back to PROBING

		SINGELINE_COMMENT does nothing and doesn't add to the output until the next newline
		MULTILINE_COMMENT does nothing and doesn't add to the output until *\/

		FALSE_CONDITIONAL ouputs nothing, and gets changed to PROBING when #endif/#else or #elif is called with correct conditional

		If the next character is a newline, it immediately goes into the NEWLINE state

		NEWLINE processes command and params if they exist, and then sends state back to PROBING_POUND_SIGN
		*/
		enum STATE { PROBING, NEWLINE, FINDING_COMMAND_START, COMMAND_NAME, PARAMETERS, STRING_LITERAL, SINGLELINE_COMMENT, MULTILINE_COMMENT};

		enum STATE state = PROBING;

		/*
		If set to true, the current character will be added to final output. This is used when an #if/#ifdef/#ifndef/#elif is called with a false condition.
		This is used because a false if condition will last multiple lines. The only other thing that doesnt output for multiple lines is a multiline comment, however it only needs to check for multiple characters.

		Instead of using outputvalue, continue should be used instead.
		*/
		bool outputValue = true;

		for (Index iter = 0; iter < input.length(); ++iter) {
			char value = (input[iter]);

			//this if statement does the backslash-newline, and converts it to a space by checking the current character is \ and the next one is \n
			if (value=='\\'&&(iter < input.length()-1 && input[iter + 1] == '\n')) {
				++iter;
				++line;
				setMacro("__LINE__", std::to_string(line));
				continue;
			}
			//if its not a backslash-newline, we check if its just a newline, obviously multiline comments ignore newlines
			else if ((value == '\n')) {
				if (state != MULTILINE_COMMENT){
					state = NEWLINE;
				}
				++line;
				setMacro("__LINE__",std::to_string(line));
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

				if (value == '\"' || value == '\'') {
					state = PROBING;
				}

				currentToken += value;
				continue;

			case SINGLELINE_COMMENT:
				continue;

			case MULTILINE_COMMENT:
				if (value == '/'&&input[iter - 1] == '*') {
					//we increment iter to account for the next / character
					state = PROBING;
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
			if (std::stoi(getMacro("__IF_SCOPE__")) > 0)throw PreprocessorException(getLocation() + ": #if directive is missing an #endif");
		}

		return tokens;
	}



	std::vector < std::string > Preprocessor::executeDirective(bool& outputValue,const std::string & command, const std::string & params)
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

				setMacro("__LINE__", std::to_string(line));
				setMacro("__FILE__", (filename));
			}
			else if (command == "define") {
				if (params.empty())throw PreprocessorException(getLocation() + ": #define must be called with a name and definition");

				Index iterator;
				std::string macroName = "";
				std::string macroDefinition = "";

				//first we get the line number
				for (iterator = 0; iterator < params.length(); ++iterator) {
					if (isspace(params[iterator])) {
						++iterator;//lets also increment the iterator so the space doesnt get added to the filename
						break;//the second word, lets go to the second loop
					}
					macroName += params[iterator];
				}

				//remaining characters are added to the filename
				for (iterator; iterator < params.length(); ++iterator) {
					macroDefinition += params[iterator];
				}

				if (macroDefinition == "")throw PreprocessorException(getLocation() + ": a macro must have a definition");


				defineMacro(macroName, macroDefinition);
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

		//these 2 commands are executed regardless of if they are inside a false scope
		if (command == "else") {

			const int macroLocation = getMacroLocation("__IF_SCOPE__");
			if (macroLocation == -1) {
				throw PreprocessorException(getLocation() + ": #else is missing #if directive");
			}
			else if (std::stoi(macros[macroLocation].second) == 0)throw PreprocessorException(getLocation() + ": #else is missing #if directive");
			else if (macros[macroLocation].second != getMacro("__CURRENT_IF_SCOPE__")){
				outputValue = !outputValue;
			}
		}
		else if (command == "endif") {

			const int macroLocation = getMacroLocation("__IF_SCOPE__");
			if (macroLocation == -1) {
				throw PreprocessorException(getLocation() + ": #endif is missing #if directive");
			}
			else if (macros[macroLocation].second == getMacro("__CURRENT_IF_SCOPE__")){
				if (macros[macroLocation].second == "0")throw PreprocessorException(getLocation() + ": #endif is missing #if directive");
				macros[macroLocation].second = std::to_string(std::stoi(macros[macroLocation].second) - 1);

				outputValue = true;
			}

		}
		else if (command == "ifdef") {
			if (params.empty())throw PreprocessorException(getLocation() + ": #ifdef directive must be called with a macro name");

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

			int macroLocation = getMacroLocation("__IF_SCOPE__");
			int currentIfScopeLocation = getMacroLocation("__CURRENT_IF_SCOPE__");


			if (macroLocation == -1) {
				//setMacro() adds a macro if its not defined, so we use it here
				setMacro("__IF_SCOPE__", "1");
				macroLocation = getMacroLocation("__IF_SCOPE__");
			}
			if (currentIfScopeLocation == -1) {
				setMacro("__CURRENT_IF_SCOPE__", macros[macroLocation].second);
				currentIfScopeLocation = getMacroLocation("__CURRENT_IF_SCOPE__");
			}
			if (macros[macroLocation].second == macros[currentIfScopeLocation].second){
				//this is faster than setMacro(), as its done in O(1) time instead of O(N) time, where N is the amount of macros
				macros[macroLocation].second = std::to_string(std::stoi(macros[macroLocation].second) + 1);

				outputValue = isMacroDefined(macroName);

				if(!outputValue)setMacro("__CURRENT_IF_SCOPE__", macros[macroLocation].second);
			}

			std::cout << macros[macroLocation].second << " " << macros[currentIfScopeLocation].second << std::endl;


		}
		else if (command == "ifndef") {
			//why write it twice when we have recursion?
			executeDirective(outputValue, "ifdef", params);

			//we reverse the output
			outputValue = !outputValue;
		}

		return std::vector < std::string >();
	}

	int Preprocessor::getMacroLocation(const std::string & name) const
	{
		for (unsigned int iterator = 0; iterator < macros.size(); ++iterator) {
			if (macros[iterator].first == name) {
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
		//so we can edit it safely without interuppting const status
		std::string token = input;

		//remove whitespace to check for macro. a line like int a = MACRO wont work without this line.
		token.erase(std::remove_if(token.begin(), token.end(), isspace), token.end());

		const int macroLocation = getMacroLocation(token);
		if (macroLocation >= 0) {
			std::string out = input;

			std::size_t index;
			//we want to preserve the whitespace in the final output, so this algorithim replaces the non-whitespace with the macro expansion
			while ((index = out.find(token)) != std::string::npos){
				out.replace(index, token.size(), macros[macroLocation].second);
			}

			return out;
		}

		return input;
	}

	void Preprocessor::setMacro(const std::string & name, const std::string & definition)
	{
		for (unsigned int iterator = 0; iterator < macros.size(); ++iterator) {
			if (macros[iterator].first == name) {
				macros[iterator].second = definition;
			}
		}
		macros.push_back(std::make_pair(name, definition));
	}


	void Preprocessor::defineMacro(const std::string & name, const std::string & definition)
	{
		for (unsigned int iterator = 0; iterator < sizeof(reservedWords) / sizeof(*reservedWords); ++iterator) {
			if (reservedWords[iterator] == name)throw PreprocessorException(getLocation()+": can\'t define a reserved word");
		}
		setMacro(name,definition);
	}

	bool Preprocessor::isMacroDefined(const std::string & name) const
	{
		for (unsigned int iterator = 0; iterator < macros.size(); ++iterator) {
			if (macros[iterator].first == name) {
				return true;
			}
		}
		return false;
	}

	void Preprocessor::undefineMacro(const std::string & name)
	{
		unsigned int iterator = 0;
		for (iterator = 0; iterator < sizeof(reservedWords) / sizeof(*reservedWords); ++iterator) {
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