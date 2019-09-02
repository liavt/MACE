/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__UTILITY_PROCESS_H
#define MACE__UTILITY_PROCESS_H

#include <MACE/Core/Constants.h>
#include <MACE/Core/Interfaces.h>

#include <string>

#ifdef MACE_POSIX
#	include <sys/types.h>
#endif

namespace mc {
	class Process: public Initializable {
	public:
		Process();
		Process(const char* path, const char* args) MACE_EXPECTS(path != nullptr && args != nullptr);
		Process(const std::string& path, std::string& args);
		// TODO make this noexcept
		~Process();

		void init() override;
		void destroy();

		int wait();

		bool isRunning() const;
		bool isInit() const noexcept override;

		void setPath(const char* p) MACE_EXPECTS(p != nullptr);
		const char* getPath() const;

		void setArgs(const char* a) MACE_EXPECTS(a != nullptr);
		const char* getArgs() const;

	private:
		bool created = false;

		const char* path;
		const char* args;

#ifdef MACE_WINAPI
		void* process = nullptr;
		void* thread = nullptr;
#elif defined(MACE_POSIX)
		pid_t process;
#endif//MACE_POSIX
	};
}

#endif//MACE__UTILITY_PROCESS_H