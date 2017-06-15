/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_UTILITY_PROCESS_H
#define MACE_UTILITY_PROCESS_H

#include <MACE/Core/Constants.h>
#include <MACE/Core/Interfaces.h>

#include <string>

#ifdef MACE_WINAPI
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#elif defined(MACE_POSIX)
#	include <unistd.h>
#endif

namespace mc {
	class Process: public Initializable {
	public:
		Process();
		Process(const char* path, const char* args);
		Process(const std::string& path, std::string& args);
		~Process();

		void init() override;
		void destroy() override;

		int wait();

		bool isRunning() const;
		bool isCreated() const;

		void setPath(const char* p);
		const char* getPath() const;

		void setArgs(const char* a);
		const char* getArgs() const;

#if defined(MACE_WINAPI) && defined(MACE_EXPOSE_WINAPI)
		PROCESS_INFORMATION getProcess() const {
			return process;
		}
#elif defined(MACE_POSIX) && defined(MACE_EXPOSE_POSIX)
		pid_t getPID() const {
			return process;
		}
#endif

	private:
		bool created = false;

		const char* path;
		const char* args;

#ifdef MACE_WINAPI
		PROCESS_INFORMATION process;
#elif defined(MACE_POSIX)
		pid_t process;
#endif//MACE_POSIX
	};
}

#endif//MACE_UTILITY_PROCESS_H