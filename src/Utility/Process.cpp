/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Utility/Process.h>
#include <MACE/Core/System.h>

#include <cstring>

#ifdef MACE_POSIX
#	include <sys/wait.h>
#	include <csignal>
#endif//MACE_POSIX

namespace mc {
	void Process::init(const char * path, char * args)
	{
		os::clearError();

		if (isCreated()) {
			throw InitializationFailedError("Can\'t call init() on an already initialized Process");
		}

#ifdef MACE_WINAPI
		STARTUPINFO startupInfo;

		ZeroMemory(&startupInfo, sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);
		ZeroMemory(&process, sizeof(process));

		//startupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
		//startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		//startupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

		CreateProcessA(path, args, nullptr, nullptr, false, 0, nullptr, nullptr, &startupInfo, &process);
#elif defined(MACE_POSIX)
		process = fork();

		if (process == 0) {
			exec(path, args);
			exit(0);
		}
		else if (process == -1) {
			os::checkError(__LINE__, __FILE__, "Failed to fork process");

			throw NullPointerError("Failed to fork process");
		}
#endif

		os::checkError(__LINE__, __FILE__, "Failed to create Process");

		created = true;

	}

	void Process::init(const std::string & path, std::string & args)
	{
		init(path.c_str(), &args[0]);//it has to be &args[0] instead of c_str() because windows requires process arguments to be mutable and c_str() is immutable.
	}
	void Process::destroy()
	{
		os::clearError();

		if (!isCreated()) {
			throw InitializationFailedError("Can\'t destroy an unitialized Process");
		}

#ifdef MACE_WINAPI
		if (isRunning()) {
			if (!TerminateProcess(process.hProcess, 1)) {
				os::checkError(__LINE__, __FILE__, "Failed to terminate process");

				throw AssertionFailedError("Failed to terminate process");
			}
		}

		if (!CloseHandle(process.hProcess)) {
			os::checkError(__LINE__, __FILE__, "Failed to close handle to process");

			throw AssertionFailedError("Failed to close handle to process");
		}

		if (!CloseHandle(process.hThread)) {
			os::checkError(__LINE__, __FILE__, "Failed to close handle to process thread");

			throw AssertionFailedError("Failed to close handle to process thread");
		}
#elif defined(MACE_POSIX)
		if (isRunning()) {
			if (!kill(process, SIGTERM)) {
				os::checkError(__LINE__, __FILE__, "Failed to send SIGTERM to process");

				throw AssertionFailedError("Failed to kill process");
			}
		}
#endif

		created = false;

		os::checkError(__LINE__, __FILE__, "Failed to destroy Process");
	}
	int Process::wait()
	{
		os::clearError(__LINE__, __FILE__);

		if (!isCreated()) {
			throw InitializationFailedError("Can\'t use wait() on unitialized Process");
		}

#ifdef MACE_WINAPI
		WaitForSingleObject(process.hProcess, INFINITE);

		os::checkError(__LINE__, __FILE__, "Error waiting for process to end");

		DWORD status;
		if (GetExitCodeProcess(process.hProcess, &status) == 0) {
			os::checkError(__LINE__, __FILE__, "Failed to retrieve exit code for process");

			throw AssertionFailedError("Failed to retrieve exit code for process");
		}

		os::checkError(__LINE__, __FILE__, "Error retrieving exit code for process");

		return static_cast<int>(status);
#elif defined(MACE_POSIX)
		int status;

		pid_t result = waitpid(process, &status, 0);

		if (result != process) {
			os::checkError(__LINE__, __FILE__, "waitpid() returned exit code " + std::to_string(result));

			throw AssertionFailedError("waitpid() returned exit code " + std::to_string(result));
		}

		os::checkError(__LINE__, __FILE__, "Error waiting for process to end");

		return status;
#endif
	}
	bool Process::isRunning() const
	{
		if (!isCreated()) {
			return false;
		}

#ifdef MACE_WINAPI
		return WaitForSingleObject(process.hProcess, 0) == WAIT_TIMEOUT;
#elif defined(MACE_POSIX)
		return kill(process, 0) == 0;
#endif
	}
	bool Process::isCreated() const
	{
		return created;
	}
	Process::Process()
	{
	}
	Process::Process(const char * path, char * args) : Process()
	{
		init(path, args);
	}
	Process::Process(const std::string & path, std::string & args) : Process()
	{
		init(path, args);
	}
	Process::~Process()
	{
		if (isCreated()) {
			destroy();
		}
	}
}//mc