/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__UTILITY_SIGNAL_H
#define MACE__UTILITY_SIGNAL_H

#include <MACE/Core/Instance.h>
#include <MACE/Core/Constants.h>
#include <exception>

namespace mc {
	namespace internal {
		template<typename ErrorType>
		class MACE__GET_ERROR_NAME(Signal): public MACE__INTERNAL_NS::Error<ErrorType, std::runtime_error> {
		public:
			using MACE__INTERNAL_NS::Error<ErrorType, std::runtime_error>::Error;
		};
	}

	namespace os {
#define MACE__SIGNAL_ERROR(name) struct name##Type { static MACE_CONSTEXPR CString val = MACE_STRINGIFY_DEFINITION(MACE__GET_ERROR_NAME(name)); }; using MACE__GET_ERROR_NAME(name) = MACE__INTERNAL_NS::MACE__GET_ERROR_NAME(Signal) < name##Type >
		/**
		Thrown when an unknown signal encountered
		*/
		MACE__SIGNAL_ERROR(SignalUnknown);

		MACE__SIGNAL_ERROR(SignalInterrupt);
		MACE__SIGNAL_ERROR(SignalTerminate);
		MACE__SIGNAL_ERROR(SignalIllegalInstruction);
		MACE__SIGNAL_ERROR(SignalAbort);
		MACE__SIGNAL_ERROR(SignalFloatingPoint);
		MACE__SIGNAL_ERROR(SignalSegmentFault);

		//the following are only thrown on POSIX systems, but can be caught from any system for portability purposes
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
			std::terminate_handler oldTerminate = nullptr;
		};
	}//os
}//mc

#endif//MACE__UTILITY_SIGNAL_H