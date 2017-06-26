#include <Catch.h>
#include <MACE/Utility/Preprocessor.h>

namespace mc {

	TEST_CASE("Testing macros", "[utility][preprocessor]") {
		SECTION("Non-functional macros") {

			SECTION("Simple replacement") {
				//we define macro to 1, and see if it expands it
				Preprocessor p = Preprocessor("#define macro 1\nmacro;");
				REQUIRE(p.preprocess() == "\n1;");
			}

			SECTION("Replacement with tabs in command") {
				Preprocessor p = Preprocessor("#\t\tdefine macro 1\nmacro;");
				REQUIRE(p.preprocess() == "\n1;");
			}

			SECTION("Stringification") {
				Preprocessor p = Preprocessor("#define foo 1\n#define macro #foo\nmacro;");
				REQUIRE(p.preprocess() == "\n\n\"1\";");
			}
		}

		SECTION("Functional macros") {

			SECTION("Parameterless function") {
				Preprocessor p = Preprocessor("#define macro() 1\nmacro();");
				REQUIRE(p.preprocess() == "\n1;");
			}

			SECTION("Function with 1 parameter") {
				Preprocessor p = Preprocessor("#define macro(a) a+1\nmacro(1);");
				REQUIRE(p.preprocess() == "\n1+1;");
			}

			SECTION("Function with 2 parameters") {
				Preprocessor p = Preprocessor("#define macro(a,b) a<<(b)\nmacro(1,31);");
				REQUIRE(p.preprocess() == "\n1<<(31);");
			}

			SECTION("Calling other macros") {
				Preprocessor p = Preprocessor("#define macro(a) a+1\n#define macro2(a) macro(a)\nmacro2(1);");
				REQUIRE(p.preprocess() == "\n\n1+1;");
			}
		}

	}

	TEST_CASE("Conditionals", "[utility][preprocessor]") {
		SECTION("Conditionals with macros") {
			SECTION("#ifdef") {
				Preprocessor p = Preprocessor("#define macro(a)\n#ifdef macro\nWorks!\n#endif\n#ifdef notAMacro\nDoesn\'t work!\n#endif");
				REQUIRE(p.preprocess() == "\n\nWorks!\n\n");
			}
			SECTION("#ifndef") {
				Preprocessor p = Preprocessor("#ifndef macro\nWorks!\n#endif\n#define macro\n#ifndef macro\nDoesn\'t work!\n#endif");
				REQUIRE(p.preprocess() == "\nWorks!\n\n\n");
			}
			SECTION("#else") {
				Preprocessor p = Preprocessor("#ifdef macro\nDoesn\'t Work!\n#else\nWorks!\n#\tendif");
				REQUIRE(p.preprocess() == "\nWorks!\n");
			}
		}

	}

	TEST_CASE("Text processing") {
		SECTION("Comments") {
			SECTION("Single line") {
				Preprocessor p = Preprocessor("Works!//this is a comment\nNext line!\n//another comment");
				REQUIRE(p.preprocess() == "Works!\nNext line!\n");
			}

			SECTION("Multi line") {
				Preprocessor p = Preprocessor("This/*This is a multiline comment\nWow!\n#error this doesn't work!\nNew line!\nIncredible!*/ works!");
				REQUIRE(p.preprocess() == "This works!");
			}
		}

		SECTION("String literals") {
			Preprocessor p = Preprocessor("#define macro 1\n\"This works! No macro expansion!\"");
			REQUIRE(p.preprocess() == "\n\"This works! No macro expansion!\"");
		}

		SECTION("Backslash newline") {
			SECTION("With directives") {
				Preprocessor p = Preprocessor("\t#define macro 1\\\n2\nmacro");
				REQUIRE(p.preprocess() == "\t\n12");
			}

			SECTION("With comments") {
				Preprocessor p = Preprocessor("//hi this is a second line \\\ncontinued comment\nHello!");
				REQUIRE(p.preprocess() == "\nHello!");
			}
		}

		SECTION("Non-macro functions") {
			SECTION("Multiline if statement") {
				Preprocessor p = Preprocessor("if(4\n&&5\n||(10\n||5)\n)");
				REQUIRE(p.preprocess() == "if(4\n&&5\n||(10\n||5)\n)");
			}

			SECTION("Normal functions") {
				Preprocessor p = Preprocessor("int main(){\nprintf(\"hi!\");\n}");
				REQUIRE(p.preprocess() == "int main(){\nprintf(\"hi!\");\n}");
			}

			SECTION("Parenthesis") {
				Preprocessor p = Preprocessor("int foo = (10)");
				REQUIRE(p.preprocess() == "int foo = (10)");
			}
		}
	}

	TEST_CASE("Testing #line", "[utility][preprocessor]") {
		Preprocessor p = Preprocessor("#line 5 testing\nthis is a line of code\n");
		REQUIRE(p.preprocess() == "\nthis is a line of code\n");
		REQUIRE(p.getLine() == 7);
		REQUIRE(p.getFilename() == "testing");
	}

	TEST_CASE("Testing #include", "[utility][preprocessor]") {
		Preprocessor p = Preprocessor("#include <test.h>");
		IncludeString s = IncludeString("test", "test.h");
		p.addInclude(s);
		REQUIRE(p.preprocess() == "\ntest");
	}

	TEST_CASE("Testing #error", "[utility][preprocessor]") {
		Preprocessor p = Preprocessor("#error This is an error!");
		REQUIRE_THROWS_AS(p.preprocess(), PreprocessorError);
	}


	TEST_CASE("#version") {
		Preprocessor p = Preprocessor("#version 330");
		REQUIRE(p.preprocess() == "#version 330");
	}

	TEST_CASE("Unknown directive") {
		Preprocessor p = Preprocessor("#test");
		REQUIRE_THROWS_AS(p.preprocess(), PreprocessorError);
	}

}