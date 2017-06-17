/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Utility/Process.h>
#include <MACE/Core/System.h>

#include <cstring>
#include <memory>

#ifdef MACE_POSIX
#	include <sys/wait.h>
#	include <signal.h>
#	include <cstdlib>
#	include <errno.h>
#endif//MACE_POSIX

namespace mc {
	void Process::init() {
		if (path == nullptr) {
			MACE__THROW(NullPointer, "The path can\'t be null!");
		} else if (args == nullptr) {
			MACE__THROW(NullPointer, "The args can\'t be null!");
		} else if (isCreated()) {
			MACE__THROW(InitializationFailed, "Can\'t call init() on an already initialized Process");
		}

		os::clearError();

#ifdef MACE_WINAPI
		STARTUPINFO startupInfo;

		ZeroMemory(&startupInfo, sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);
		ZeroMemory(&process, sizeof(process));

		startupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
		startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		startupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

		/*
		Ok let me rant for a second here. CreateProcessA does not accept a const char* for its args.
		THIS IS A MASSIVE PROBLEM
		args has to be const char* because of the possibility that someone will initialize a process
		with a string literal. In fact, it is most likely going to be a string literal, like so:
		Process proc = Process("myprog", "-a --argument");
		This can't be ignored because then G++ and clang++ raise a bunch of warnings how its a deprecated conversion
		from string literal (const char*) to character array (char*)
		I mean I agree with them. You should never remove constness of a variable...
		Meaning the function declaration has to say that args is a const char*...
		BUT WINDOWS DECIDED THAT THAT IS A BAD IDEA
		POSIX is somehow reasonable by allowing const char* in the execl function but NO WINDOWS
		CANT DO THAT
		So we have to do this dumb method of copying the const char* into a temporary mutable
		buffer for the string so windows can have its way.
		This is why Linux will always be the superior platform for development, Microsoft.
		Because of things like this.
		*/

		CreateProcess(TEXT((std::string(path) + std::string(args)).c_str()), nullptr, nullptr, nullptr, false, 0, nullptr, nullptr, &startupInfo, &process);
#elif defined(MACE_POSIX)
		process = fork();

		if (process == 0) {
			execl(path, args, nullptr);
			std::exit(-1);
		} else if (process == -1) {
			os::checkError(__LINE__, __FILE__, "Failed to fork process");

			MACE__THROW(NullPointer, "Failed to fork process");
		}
#endif

		os::checkError(__LINE__, __FILE__, "Failed to create Process");

		created = true;

	}

	void Process::destroy() {
		os::clearError();

		if (!isCreated()) {
			MACE__THROW(InitializationFailed, "Can\'t destroy an unitialized Process");
		}

#ifdef MACE_WINAPI
		if (isRunning()) {
			//hasta la vista baby
			if (!TerminateProcess(process.hProcess, 1)) {
				os::checkError(__LINE__, __FILE__, "Failed to terminate process");

				MACE__THROW(AssertionFailed, "Failed to terminate process");
			}
		}

		if (!CloseHandle(process.hProcess)) {
			os::checkError(__LINE__, __FILE__, "Failed to close handle to process");

			MACE__THROW(AssertionFailed, "Failed to close handle to process");
		}

		if (!CloseHandle(process.hThread)) {
			os::checkError(__LINE__, __FILE__, "Failed to close handle to process thread");

			MACE__THROW(AssertionFailed, "Failed to close handle to process thread");
		}
#elif defined(MACE_POSIX)
		int result = kill(process, SIGTERM);
		if (result != 0) {
			if (errno == ESRCH) {
				//meaning the process never even existed or was stopped already.
				os::clearError();
			} else {
				os::checkError(__LINE__, __FILE__, "Failed to send SIGTERM to process with result " + std::to_string(result));

				MACE__THROW(AssertionFailed, "Failed to kill process");

			}
		}

#endif

		created = false;

		os::checkError(__LINE__, __FILE__, "Failed to destroy Process");
	}

	int Process::wait() {
		os::clearError(__LINE__, __FILE__);

		if (!isCreated()) {
			MACE__THROW(InitializationFailed, "Can\'t use wait() on unitialized Process");
		}

#ifdef MACE_WINAPI
		WaitForSingleObject(process.hProcess, INFINITE);

		os::checkError(__LINE__, __FILE__, "Error waiting for process to end");

		DWORD status;
		if (GetExitCodeProcess(process.hProcess, &status) == 0) {
			os::checkError(__LINE__, __FILE__, "Failed to retrieve exit code for process");

			MACE__THROW(AssertionFailed, "Failed to retrieve exit code for process");
		}

		os::checkError(__LINE__, __FILE__, "Error retrieving exit code for process");

		return static_cast<int>(status);
#elif defined(MACE_POSIX)
		int status;

		const pid_t result = waitpid(process, &status, WUNTRACED | WCONTINUED);
		if (result != process) {
			os::checkError(__LINE__, __FILE__, "waitpid() returned exit code " + std::to_string(result));

			MACE__THROW(AssertionFailed, "waitpid() returned exit code " + std::to_string(result));
		}

		os::checkError(__LINE__, __FILE__, "Error waiting for process to end");

		if (WIFEXITED(status)) {
			return WEXITSTATUS(status);
		} else if (WIFSIGNALED(status)) {
			return WTERMSIG(status);
		} else if (WIFSTOPPED(status)) {
			return WSTOPSIG(status);
		}

		os::checkError(__LINE__, __FILE__, "Error getting error code of process");

		return status;
#endif
	}

	bool Process::isRunning() const {
		if (!isCreated()) {
			return false;
		}

#ifdef MACE_WINAPI
		return WaitForSingleObject(process.hProcess, 0) == WAIT_TIMEOUT;
#elif defined(MACE_POSIX)
		return kill(process, 0) == 0;
#endif
	}

	bool Process::isCreated() const {
		return created;
	}

	void Process::setPath(const char * p) {
		path = p;
	}

	const char * Process::getPath() const {
		return path;
	}

	void Process::setArgs(const char * a) {
		args = a;
	}

	const char * Process::getArgs() const {
		return args;
	}


	Process::Process(const char * p, const char * a) : path(p), args(a) {}

	Process::Process(const std::string & path, std::string & args) : Process(path.c_str(), args.c_str()) {}

	Process::Process() : Process(nullptr, nullptr) {}

	Process::~Process() {
		if (isCreated()) {
			destroy();
		}
	}
}//mc
