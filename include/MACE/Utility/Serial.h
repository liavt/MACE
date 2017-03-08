/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_UTILITY_SERIAL_H
#define MACE_UTILITY_SERIAL_H

#include <MACE/Core/Constants.h>
#include <MACE/Utility/BitField.h>

#include <string>

#ifdef MACE_WINAPI
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif

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
			enum SerialProperty: Byte {
				READ = 0,
				WRITE = 1,
			};

			Serial(const char* port, const Size baudRate = 9600);
			Serial(const std::string& port, const Size baudRate = 9600);
			~Serial();

			void init(const char* port,const Size baudRate = 9600);
			void destroy();

			int read(char* buffer, Size bufferSize);
			template<int S>
			int read(char buffer[S]) {
				return this->read(buffer, S);
			}

			void write(const char* buffer, const Size bufferSize);
			template<int S>
			int write(const char buffer[S]) {
				return this->write(buffer, S);
			}
			void write(const std::string& port);

			void flush();

			Size getAvailableCharacterAmount() const;

			bool hasAvailable() const;

			bool isConnected() const;
			bool isValid() const;
		private:
			bool connected = false;

#ifdef MACE_WINAPI
			//file handle
			HANDLE serial;
#elif defined(MACE_POSIX)
			//file descriptor
			int serial;
#endif
		};
	}//os
}//mc

#endif//MACE_UTILITY_SERIAL_H
