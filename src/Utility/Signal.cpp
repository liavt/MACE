/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Utility/Signal.h>

#include <csignal>

namespace mc {
	namespace os {
		void signalHandle(int sig) {
#ifdef MACE_POSIX
			if( sig == SIGHUP ) {
				throw SignalHangupError("SIGHUP: Hangup detected on controlling terminal or death of controlling process");
			} else if( sig == SIGKILL ) {
				//RIP program
				throw SignalKillError("SIGKILL: Program was killed");
			} else if( sig == SIGSTOP ) {
				throw SignalStopError("SIGSTOP: Program was stopped");
			} else if( sig == SIGALRM ) {
				throw SignalAlarmError("SIGALRM: Abort signal from alarm()");
			} else if( sig == SIGTSTP ) {
				throw SignalTerminalStopError("SIGTSTP: Stop was typed in the terminal");
			} else if( sig == SIGTTIN ) {
				throw SignalTerminalInputError("SIGTTIN: Terminal input for background process");
			} else if( sig == SIGTTOU ) {
				throw SignalTerminalOutputError("SIGTTOU: Terminal output for background process");
			}
#endif//MACE_POSIX

			if( sig == SIGABRT
#ifdef MACE_WINDOWS
			   || sig == SIGABRT_COMPAT
#endif//MACE_WINDOWS
			   ) {
				throw SignalAbortError("SIGABRT: Program was aborted");
			} else if( sig == SIGFPE ) {
				throw SignalFloatingPointError("SIGFPE: A floating point error occured");
			} else if( sig == SIGILL ) {
				throw SignalIllegalInstructionError("SIGILL: An illegal instruction occured");
			} else if( sig == SIGINT ) {
				throw SignalInterruptError("SIGINT: Program was interrupted from keyboard");
			} else if( sig == SIGSEGV ) {
				throw SignalSegmentFaultError("SIGSEGV: Invalid memory reference (segmentation fault)");
			} else if( sig == SIGTERM ) {
				throw SignalTerminateError("SIGTERM: Program was terminated");
			} else {
				throw SignalError("Program recieved signal " + sig);
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
	}//os
}//mc