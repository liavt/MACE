#include <Catch.h>
#include <MACE/Utility/SignalModule.h>

#include <csignal>

namespace mc {
	namespace os {
		TEST_CASE("Testing signal handling") {
			SignalModule m = SignalModule();

			MACE::addModule(m);

			MACE::init();

			REQUIRE_THROWS_AS(std::raise(SIGABRT), SignalAbortError);
			REQUIRE_THROWS_AS(std::raise(SIGINT), SignalInterruptError);
			REQUIRE_THROWS_AS(std::raise(SIGILL), SignalIllegalInstructionError);
			REQUIRE_THROWS_AS(std::raise(SIGFPE), SignalFloatingPointError);
			REQUIRE_THROWS_AS(std::raise(SIGSEGV), SignalSegmentFaultError);
			REQUIRE_THROWS_AS(std::raise(SIGTERM), SignalTerminateError);

#ifdef MACE_POSIX
			REQUIRE_THROWS_AS(std::raise(SIGHUP), SignalHangupError);
			REQUIRE_THROWS_AS(std::raise(SIGKILL), SignalKillError);
			REQUIRE_THROWS_AS(std::raise(SIGSTOP), SignalStopError);
			REQUIRE_THROWS_AS(std::raise(SIGALRM), SignalAlarmError);
			REQUIRE_THROWS_AS(std::raise(SIGTSTP), SignalTerminalStopError);
			REQUIRE_THROWS_AS(std::raise(SIGTTIN), SignalTerminalInputError);
			REQUIRE_THROWS_AS(std::raise(SIGTTOU), SignalTerminalOutputError);
#endif//MACE_POSIX

			MACE::destroy();
		}
	}//os
}//mc