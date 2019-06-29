/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__UTILITY_SERIAL_H
#define MACE__UTILITY_SERIAL_H

#include <MACE/Core/Constants.h>

#include <string>

namespace mc {
	namespace os {
		/**
		Opens up a serial communciation.
		<p>
		The serial connection will have no parity and a byte size of 8. The baud rate can be specified by the constructor.
		<p>
		Data will be sent and received raw, with no extra formatting.
		<p>
		The `Serial` class is intended for communication to a microcontroller such as an Arduino.
		*/
		class Serial {
		public:
			Serial();
			Serial(const char* port, const unsigned int baudRate = 9600);
			Serial(const std::string& port, const unsigned int baudRate = 9600);
			// TODO make this noexcept
			~Serial();

			void init(const char* port, const unsigned int baudRate = 9600);
			void destroy();

			int read(char* buffer, unsigned int bufferSize);
			template<int S>
			int read(char buffer[S]) {
				return this->read(buffer, S);
			}

			void write(const char* buffer, const unsigned int bufferSize);
			template<int S>
			int write(const char buffer[S]) {
				return this->write(buffer, S);
			}
			void write(const std::string& port);

			void flush();

			unsigned int getAvailableCharacterAmount() const;

			bool hasAvailable() const;

			bool isConnected() const;
			bool isValid() const;
		private:
			bool connected = false;

#ifdef MACE_WINAPI
			//file handle
			void* serial;
#elif defined(MACE_POSIX)
			//file descriptor
			int serial;
#endif
		};
	}//os
}//mc

#endif//MACE__UTILITY_SERIAL_H
