/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <catch2/catch.hpp>
#include <MACE/Utility/DynamicLibrary.h>


namespace mc {
	extern "C" MACE_FUNCTION_EXPORT int exportTest1(int returnValue) {
		return returnValue;
	}

	extern "C" MACE_FUNCTION_EXPORT bool exportTest2() {
		return true;
	}

	typedef int(*ExportTest1Ptr)(int);
	typedef bool(*ExportTest2Ptr)();

	//this macro is defined when the file is compiled as a dynamic library
#ifndef MACE_DLL_OUT_PASS
	TEST_CASE("Testing dynamic linking with getRunningProcess()", "[utility][dynamiclibrary]") {
		DynamicLibrary runningProcess = DynamicLibrary::getRunningProcess();
		
		REQUIRE(runningProcess.isCreated());
		
		ExportTest1Ptr exportTest1Func = reinterpret_cast<ExportTest1Ptr>(runningProcess.getSymbol("exportTest1"));

		REQUIRE(exportTest1Func(42) == 42);

		REQUIRE(runningProcess.isCreated());

		ExportTest2Ptr exportTest2Func = reinterpret_cast<ExportTest2Ptr>(runningProcess["exportTest2"]);

		REQUIRE(exportTest2Func());

		REQUIRE(exportTest1Func(42) == 42);
		
		REQUIRE(runningProcess.isCreated());
		
		runningProcess.destroy();
		
		REQUIRE(!runningProcess.isCreated());
	}

	TEST_CASE("Testing loading dynamic libraries from file system", "[utility][dynamiclibrary]") {
		//for this demo, this source file is compiled twice: one as part as the testing executable, and one
		//as the platform specific shared library. cmake handles all of that. MACE_DLL_TEST_OUTPUT is defined
		//by cmake to wherever the library is compiled to.
		DynamicLibrary dllTest = DynamicLibrary(MACE_DLL_TEST_OUTPUT);

		REQUIRE(dllTest.isCreated());

		ExportTest1Ptr exportTest1Func = reinterpret_cast<ExportTest1Ptr>(dllTest.getSymbol("exportTest1"));

		REQUIRE(exportTest1Func(42) == 42);

		REQUIRE(dllTest.isCreated());

		ExportTest2Ptr exportTest2Func = reinterpret_cast<ExportTest2Ptr>(dllTest["exportTest2"]);

		REQUIRE(exportTest2Func());

		REQUIRE(exportTest1Func(42) == 42);

		REQUIRE(dllTest.isCreated());

		dllTest.destroy();

		REQUIRE(!dllTest.isCreated());
	}
#endif//MACE_DLL_OUT_PASS
}//mc
