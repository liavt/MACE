/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__UTILITY_SIGNAL_H
#define MACE__UTILITY_SIGNAL_H

#include <MACE/Core/Instance.h>
#include <MACE/Core/Constants.h>
#include <exception>

namespace mc {
	namespace os {
		class MACE__GET_ERROR_NAME(Signal): public Error {
		public:
			using Error::Error;
		};

#define MACE__SIGNAL_ERROR(name) class MACE__GET_ERROR_NAME(name) : public  MACE__GET_ERROR_NAME(Signal) {public: using MACE__GET_ERROR_NAME(Signal) :: MACE__GET_ERROR_NAME(Signal) ;}
		MACE__SIGNAL_ERROR(SignalInterrupt);
		MACE__SIGNAL_ERROR(SignalTerminate);
		MACE__SIGNAL_ERROR(SignalIllegalInstruction);
		MACE__SIGNAL_ERROR(SignalAbort);
		MACE__SIGNAL_ERROR(SignalFloatingPoint);
		MACE__SIGNAL_ERROR(SignalSegmentFault);

		//the following are only thrown on POSIX systems, but can be caught from any system just in case
		MACE__SIGNAL_ERROR(SignalHangup);
		MACE__SIGNAL_ERROR(SignalKill);
		MACE__SIGNAL_ERROR(SignalStop);
		MACE__SIGNAL_ERROR(SignalAlarm);
		MACE__SIGNAL_ERROR(SignalTerminalStop);
		MACE__SIGNAL_ERROR(SignalTerminalInput);
		MACE__SIGNAL_ERROR(SignalTerminalOutput);

#undef MACE__SIGNAL_ERROR//MACE__SIGNAL_ERROR

		class SignalModule: public Module {
		public:
			virtual void init() override;
			virtual void update() override;
			virtual void destroy() override;

			virtual std::string getName() const override;
		};//SignalModule

		class ErrorModule: public SignalModule {
		public:
			void init() override;
			void update() override;
			void destroy() override;

			std::string getName() const override;
		private:
			std::unexpected_handler oldUnexpected;
			std::terminate_handler oldTerminate;
		};
	}//os
}//mc

#endif//MACE__UTILITY_SIGNAL_H