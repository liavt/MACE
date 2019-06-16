/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Utility/Signal.h>

#include <iostream>
#include <csignal>

namespace mc {
	namespace os {
		namespace {
			void signalHandle [[noreturn]] (int sig) {
				//the following code is pretty boring and self explanatory.
				//plus signals are literally the devil to debug, especially SIGSEGV
				//so to lighten up the mood for developers that encounter these signals,
				//i've written a "joke" about every single one
				try {
#ifdef MACE_POSIX
					if (sig == SIGHUP) {
						//no YOU hangup first!
						MACE__THROW(SignalHangup, "SIGHUP: Hangup detected on controlling terminal or death of controlling process");
					} else if (sig == SIGKILL) {
						//RIP program. may it forever live on in /dev/null
						MACE__THROW(SignalKill, "SIGKILL: Program was killed");
					} else if (sig == SIGSTOP) {
						//stop, hammertime
						MACE__THROW(SignalStop, "SIGSTOP: Program was stopped");
					} else if (sig == SIGALRM) {
						//the terminal's form of birth control
						MACE__THROW(SignalAlarm, "SIGALRM: Abort signal from alarm()");
					} else if (sig == SIGTSTP) {
						//ive got a terminal illness
						MACE__THROW(SignalTerminalStop, "SIGTSTP: Stop was typed in the terminal");
					} else if (sig == SIGTTIN) {
						//the adventures of TTIN TTIN
						MACE__THROW(SignalTerminalInput, "SIGTTIN: Terminal input for background process");
					} else if (sig == SIGTTOU) {
						//no TTOU
						MACE__THROW(SignalTerminalOutput, "SIGTTOU: Terminal output for background process");
					}
#endif//MACE_POSIX

					if (sig == SIGABRT
#ifdef MACE_WINDOWS
						//SIBABRT_COMPAT is the same as SIGABRT on Windows platforms.
						//It exists for compatibilty with POSIX platforms
						|| sig == SIGABRT_COMPAT
#endif//MACE_WINDOWS
						) {
						//conservatives hate SIGABRT
						MACE__THROW(SignalAbort, "SIGABRT: Program was aborted");
					} else if (sig == SIGFPE) {
						//i was just trying to make a point
						MACE__THROW(SignalFloatingPoint, "SIGFPE: A floating point error occured");
					} else if (sig == SIGILL) {
						//oh dear, the program seems quite ill
						MACE__THROW(SignalIllegalInstruction, "SIGILL: An illegal instruction occured");
					} else if (sig == SIGINT) {
						//its rude to interuppt
						MACE__THROW(SignalInterrupt, "SIGINT: Program was interrupted from keyboard");
					} else if (sig == SIGSEGV) {
						//now to unleash the ol' gdb and empty the next 3 days from the calendar
						MACE__THROW(SignalSegmentFault, "SIGSEGV: Invalid memory reference (segmentation fault)");
					} else if (sig == SIGTERM) {
						//hasta la vista baby
						MACE__THROW(SignalTerminate, "SIGTERM: Program was terminated");
					}  else MACE_UNLIKELY{
						//we are getting mixed signals here
						MACE__THROW(SignalUnknown, "Program recieved signal " + std::to_string(sig));
					}
				} catch (const std::exception& err) {
					handleError(err);
				}
			}

			void onUnexpected [[noreturn]] () noexcept {
				try {
					handleError(MACE__GET_ERROR_NAME(Unknown) ("An unexpected error occured", MACE_STRINGIFY_DEFINITION(__LINE__), __FILE__));
				} catch (...) {
					std::cerr << "onUnexpected(): An unexpected error occured trying to show an unexpected error" << std::endl;
					//this function should never throw an error, ignore all errors and exit anyways
				}
				std::exit(EXIT_FAILURE);
			}

			void onTerminate [[noreturn]] () noexcept{
				try {
					handleError(MACE__GET_ERROR_NAME(Unknown) ("An exception was thrown somewhere and not caught appropriately", MACE_STRINGIFY_DEFINITION(__LINE__), __FILE__));
				} catch (...) {
					std::cerr << "onTerminate(): An unexpected error occured trying to show an error" << std::endl;
					//this function should never throw an error, ignore all errors and exit anyways
				}
				std::exit(EXIT_FAILURE);
			}
		}

		void SignalModule::init() {
			std::signal(SIGABRT, &signalHandle);
			std::signal(SIGFPE, &signalHandle);
			std::signal(SIGILL, &signalHandle);
			std::signal(SIGINT, &signalHandle);
			std::signal(SIGSEGV, &signalHandle);
			std::signal(SIGTERM, &signalHandle);

#ifdef MACE_WINDOWS
			std::signal(SIGABRT_COMPAT, &signalHandle);
#elif defined(MACE_POSIX)
			std::signal(SIGHUP, &signalHandle);
			std::signal(SIGKILL, &signalHandle);
			std::signal(SIGSTOP, &signalHandle);
			std::signal(SIGALRM, &signalHandle);
			std::signal(SIGTSTP, &signalHandle);
			std::signal(SIGTTIN, &signalHandle);
			std::signal(SIGTTOU, &signalHandle);
#endif//MACE_POSIX
		}

		void SignalModule::update() {}

		void SignalModule::destroy() {
			std::signal(SIGABRT, SIG_DFL);
			std::signal(SIGFPE, SIG_DFL);
			std::signal(SIGILL, SIG_DFL);
			std::signal(SIGINT, SIG_DFL);
			std::signal(SIGSEGV, SIG_DFL);
			std::signal(SIGTERM, SIG_DFL);
#ifdef MACE_POSIX
			std::signal(SIGHUP, SIG_DFL);
			std::signal(SIGKILL, SIG_DFL);
			std::signal(SIGSTOP, SIG_DFL);
			std::signal(SIGALRM, SIG_DFL);
			std::signal(SIGTSTP, SIG_DFL);
			std::signal(SIGTTIN, SIG_DFL);
			std::signal(SIGTTOU, SIG_DFL);
#endif//MACE_POSIX
		}

		std::string SignalModule::getName() const {
			return "MACE/Signal";
		}

		void ErrorModule::init() {
			oldTerminate = std::set_terminate(&onTerminate);

			SignalModule::init();
		}

		void ErrorModule::update() {
			SignalModule::update();
		}

		void ErrorModule::destroy() {
			SignalModule::destroy();

			std::set_terminate(oldTerminate);
		}

		std::string ErrorModule::getName() const {
			return "MACE/Error";
		}
	}//os
}//mc
