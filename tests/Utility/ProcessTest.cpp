/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#ifdef MACE_PROC_OUT_PASS
int main() {
	//5 is a nice number to return because its unsigned and not 1 or 0
	return 5;
}
#else//MACE_PROC_OUT_PASS
#include <catch2/catch.hpp>
#define MACE_EXPOSE_WINAPI
#define MACE_EXPOSE_POSIX
#include <MACE/Utility/Process.h>
#include <cstring>

namespace mc {
	TEST_CASE("Testing Process class", "[utility][process]") {
		Process p = Process(MACE_PROC_TEST_OUTPUT, "");

		REQUIRE(std::strcmp(p.getPath(), MACE_PROC_TEST_OUTPUT) == 0);
		REQUIRE(std::strcmp(p.getArgs(), "") == 0);

		REQUIRE_FALSE(p.isRunning());

		REQUIRE_FALSE(p.isInit());

		p.init();

		REQUIRE(p.isRunning());

		REQUIRE(p.isInit());

		REQUIRE(p.wait() == 5);

		REQUIRE_FALSE(p.isRunning());

		REQUIRE(p.isInit());

		p.destroy();

		REQUIRE_FALSE(p.isInit());
		REQUIRE_FALSE(p.isRunning());
	}
}//mc
#endif
