/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_UTILITY_SIGNAL_H
#define MACE_UTILITY_SIGNAL_H

#include <MACE/Core/Module.h>
#include <MACE/Core/Constants.h>

namespace mc {
	namespace os {
		class SignalError: public Error {
		public:
			using Error::Error;
		};

#define MACE__SIGNAL_ERROR(name) class Signal##name##Error : public SignalError{public: using SignalError::SignalError;}
		MACE__SIGNAL_ERROR(Interrupt);
		MACE__SIGNAL_ERROR(Terminate);
		MACE__SIGNAL_ERROR(IllegalInstruction);
		MACE__SIGNAL_ERROR(Abort);
		MACE__SIGNAL_ERROR(FloatingPoint);
		MACE__SIGNAL_ERROR(SegmentFault);

		//the following are only thrown on POSIX systems, but can be caught from any system just in case
		MACE__SIGNAL_ERROR(Hangup);
		MACE__SIGNAL_ERROR(Kill);
		MACE__SIGNAL_ERROR(Stop);
		MACE__SIGNAL_ERROR(Alarm);
		MACE__SIGNAL_ERROR(TerminalStop);
		MACE__SIGNAL_ERROR(TerminalInput);
		MACE__SIGNAL_ERROR(TerminalOutput);

#undef MACE__SIGNAL_ERROR//MACE__SIGNAL_ERROR

		void signalHandle[[noreturn]](int sig);

		class SignalModule: public Module {
		public:
			void init() override;
			void update() override;
			void destroy() override;

			std::string getName() const override;
		};//SignalModule
	}//os
}//mc

#endif//MACE_UTILITY_SIGNAL_H