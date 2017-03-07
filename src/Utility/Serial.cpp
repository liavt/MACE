/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Utility/Serial.h>
#include <MACE/Core/System.h>

namespace mc {
	namespace os {
		Serial::Serial(const char * port, const BitField & flags, const Size baudRate) {
			init(port, flags, baudRate);
		}

		Serial::Serial(const std::string & port, const BitField & flags, const Size baudRate) : Serial(port.c_str(), flags, baudRate) {}

		Serial::~Serial() {
			if( isConnected() ) {
				destroy();
			}
		}

		void Serial::init(const char * port, const BitField & flags, const Size baudRate) {
			if( isConnected() ) {
				throw AssertionFailedError("Can't call init() on a Serial class that is already connected!");
			}

#ifdef MACE_WINAPI
			DWORD accessFlags;
			if( flags.getBit(Serial::WRITE) && flags.getBit(Serial::READ) ) {
				accessFlags = GENERIC_READ | GENERIC_WRITE;
			} else if( flags.getBit(Serial::READ) ) {
				accessFlags = GENERIC_READ;
			} else if( flags.getBit(Serial::WRITE) ) {
				accessFlags = GENERIC_WRITE;
			} else {
				throw InitializationFailedError("Serial port must be opened with either the Serial::WRITE or Serial::READ flags!");
			}

			serial = CreateFile(port, accessFlags, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

			if( serial == INVALID_HANDLE_VALUE ) {
				const DWORD lastError = GetLastError();

				if( lastError == ERROR_FILE_NOT_FOUND ) {
					throw FileNotFoundError("Serial port with name " + std::string(port) + " not found.");
				} else {
					throw FileNotFoundError("Error opening " + std::string(port) + " with error from CreateFile " + std::to_string(lastError));
				}
			}

			DCB serialParams = { 0 };

			if( !GetCommState(serial, &serialParams) ) {
				throw BadFileError("Error getting serial parameters from " + std::string(port));
			}

			serialParams.BaudRate = baudRate;
			serialParams.ByteSize = 8;
			serialParams.StopBits = ONESTOPBIT;
			serialParams.Parity = NOPARITY;
			serialParams.fDtrControl = DTR_CONTROL_ENABLE;

			if( !SetCommState(serial, &serialParams) ) {
				throw BadFileError("Error serial parameters for " + std::string(port));
			}

			//we shall purge everything to flush any previous characters;
			PurgeComm(serial, PURGE_RXCLEAR | PURGE_TXCLEAR);

			os::checkError(__LINE__, __FILE__);

			connected = true;
#elif defined(MACE_POSIX)
			mode_t accessFlags;
			if( flags.getBit(Serial::WRITE) && flags.getBit(Serial::READ) ) {
				accessFlags = S_IRWXU;
			} else if( flags.getBit(Serial::READ) ) {
				accessFlags = S_IRUSR;
			} else if( flags.getBit(Serial::WRITE) ) {
				accessFlags = S_IWUSR;
			} else {
				throw InitializationFailedError("Serial port must be opened with either the Serial::WRITE or Serial::READ flags!");
			}

			//tell it to not block or own the process
			serial = open(port, O_RDWR | O_NOCTTY | O_NDELAY, accessFlags);

			if( serial == -1 ) {
				throw FileNotFoundError("Unable to open serial port " + std::string(port));
			}

			//we dont want to block reading - only return 0.
			fcntl(serial, F_SETFL, FNDELAY);

			struct termios options;

			//get current options
			tcgetattr(serial, &options);

			cfsetispeed(&options, baudRate);
			cfsetospeed(&options, baudRate);

			options.c_cflag |= (CLOCAL | CREAD);

			//enable no parity and 8 byte word size
			options.c_cflag &= ~PARENB
				options.c_cflag &= ~CSTOPB
				options.c_cflag &= ~CSIZE;
			options.c_cflag |= CS8;

			//raw input mode
			options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

			//raw output mode
			options.c_oflag &= ~OPOST;

			//actually set the options
			tcsetattr(serial, TCSANOW, &options);

			os::checkError(__LINE__, __FILE__);

			connected = true;
#endif
		}

		void Serial::destroy() {
			if( !connected ) {
				throw AssertionFailedError("Can't destroy an unconnected Serial object!");
			}

			connected = false;

#ifdef MACE_WINAPI
			CloseHandle(serial);
#elif defined(MACE_POSIX)
			close(serial);
#endif

			os::checkError(__LINE__, __FILE__);
		}

		int Serial::read(char * buffer, Size bufferSize) {
			if( !isConnected() ) {
				throw AssertionFailedError("Failed to read from serial: Not connected to serial port");
			}

			Size available = getAvailableCharacterAmount();

			if( available < 1 ) {
				return 0;
			}

			Size toRead;

			if( available > bufferSize ) {
				toRead = bufferSize;
			} else {
				toRead = available;
			}

#ifdef MACE_WINAPI
			DWORD bytesRead;

			if( ReadFile(serial, buffer, toRead, &bytesRead, nullptr) ) {
				return bytesRead;
			}

			os::checkError(__LINE__, __FILE__);

			return 0;
#elif defined(MACE_POSIX)
			ssize_t bytesRead = read(serial, static_cast<void*>(buffer), toRead);

			if( bytesRead < 0 ) {
				os::checkError(__LINE__, __FILE__);
			}

			return bytesRead;
#endif
		}

		void Serial::write(const char * buffer, const Size bufferSize) {
			if( !isConnected() ) {
				throw AssertionFailedError("Failed to write to serial: Not connected to serial port");
			}

#ifdef MACE_WINAPI
			DWORD bytesSent;

			if( !WriteFile(serial, static_cast<const void*>(buffer), bufferSize, &bytesSent, 0) ) {
				os::checkError(__LINE__, __FILE__);
			}

			return;
#elif defined(MACE_POSIX)
			ssize_t bytesWritten = write(serial, static_cast<const void*>(buffer), bufferSize, 0);

			if( bytesWritten < 0 ) {
				os::checkError(__LINE__, __FILE__);
				return;
			}
#endif
		}

		void Serial::write(const std::string & port) {
			write(port.c_str(), port.size());
		}

		void Serial::flush() {
#ifdef MACE_WINAPI
			FlushFileBuffers(serial);

			os::checkError(__LINE__, __FILE__);
#elif defined(MACE_POSIX)
			tcflush(serial, TCIOFLUSH);

			os::checkError(__LINE__, __FILE__);
#endif
		}

		Size Serial::getAvailableCharacterAmount() const {
#ifdef MACE_WINAPI
			DWORD errors;
			COMSTAT status;

			ClearCommError(serial, &errors, &status);

			os::checkError(__LINE__, __FILE__);

			return status.cbInQue;
#elif defined(MACE_POSIX)
			int bytes;

			ioctl(serial, FIONREAD, &bytes);

			os::checkError(__LINE__, __FILE__);

			return static_cast<Size>(bytes);
#endif
		}

		bool Serial::hasAvailable() const {
			return getAvailableCharacterAmount() > 0;
		}

		bool Serial::isConnected() const {
			return connected;
		}
	}//os
}//mc
