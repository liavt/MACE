/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Utility/Serial.h>
#include <MACE/Core/System.h>

#ifdef MACE_POSIX
#	include <sys/stat.h>
#	include <sys/ioctl.h>
#	include <fcntl.h>
#	include <termios.h>
#	include <unistd.h>
#endif

namespace mc {
	namespace os {
		Serial::Serial() {}

		Serial::Serial(const char * port, const Size baudRate) : Serial() {
			init(port, baudRate);
		}

		Serial::Serial(const std::string & port, const Size baudRate) : Serial(port.c_str(), baudRate) {}

		Serial::~Serial() {
			if (isConnected()) {
				destroy();
			}
		}

		void Serial::init(const char * port, const Size baudRate) {
			os::clearError();

			if (isConnected()) {
				MACE__THROW(AssertionFailed, "Can't call init() on a Serial class that is already connected!");
			}

#ifdef MACE_WINAPI
			serial = CreateFile(TEXT(port), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

			if (serial == INVALID_HANDLE_VALUE) {
				const DWORD lastError = GetLastError();

				if (lastError == ERROR_FILE_NOT_FOUND) {
					MACE__THROW(FileNotFound, "Serial port with name " + std::string(port) + " not found.");
				} else if (lastError == ERROR_ACCESS_DENIED) {
					throw SystemError("Access denied accessing serial port at " + std::string(port));
				} else {
					MACE__THROW(FileNotFound, "Error opening " + std::string(port) + " with error from CreateFile " + std::to_string(lastError));
				}
			}

			os::checkError(__LINE__, __FILE__, "Error opening serial socket with CreateFile at " + std::string(port));

			DCB serialParams = { 0 };

			if (!GetCommState(serial, &serialParams)) {
				throw BadFileError("Error getting serial parameters from " + std::string(port));
			}

			serialParams.BaudRate = baudRate;
			serialParams.ByteSize = 8;
			serialParams.StopBits = ONESTOPBIT;
			serialParams.Parity = NOPARITY;
			serialParams.fDtrControl = DTR_CONTROL_ENABLE;

			if (!SetCommState(serial, &serialParams)) {
				throw BadFileError("Error setting serial parameters for " + std::string(port));
			}

			os::checkError(__LINE__, __FILE__, "Error setting serial communication status for " + std::string(port));

			COMMTIMEOUTS timeout = { 0 };
			timeout.ReadIntervalTimeout = 50;
			timeout.ReadTotalTimeoutConstant = 50;
			timeout.ReadTotalTimeoutMultiplier = 50;
			timeout.WriteTotalTimeoutConstant = 50;
			timeout.WriteTotalTimeoutMultiplier = 10;

			if (!SetCommTimeouts(serial, &timeout)) {
				throw BadFileError("Error setting serial timeout for " + std::string(port));
			}

			os::checkError(__LINE__, __FILE__, "Error setting serial communication timeouts for " + std::string(port));

			//we shall purge everything to flush any previous characters;
			if (!PurgeComm(serial, PURGE_RXCLEAR | PURGE_TXCLEAR)) {
				throw BadFileError("Error purging serial communications for " + std::string(port));
			};

			os::checkError(__LINE__, __FILE__, "Error purging previous messages on the serial communication line at " + std::string(port));
#elif defined(MACE_POSIX)
			//tell it to not block or own the process
			serial = open(port, O_RDWR | O_NOCTTY | O_NDELAY, S_IRWXU);

			if (serial == -1) {
				MACE__THROW(FileNotFound, "Unable to open serial port " + std::string(port));
			}

			os::checkError(__LINE__, __FILE__, "Error opening serial port at " + std::string(port));

			//we dont want to block reading - only return 0.
			fcntl(serial, F_SETFL, FNDELAY);

			os::checkError(__LINE__, __FILE__, "Error setting serial block settings for " + std::string(port));

			struct termios options;

			//get current options
			tcgetattr(serial, &options);

			os::checkError(__LINE__, __FILE__, "Error getting serial attributes for " + std::string(port));

			cfsetispeed(&options, baudRate);
			cfsetospeed(&options, baudRate);

			os::checkError(__LINE__, __FILE__, "Error setting baud rate for " + std::string(port));

			options.c_cflag |= (CLOCAL | CREAD);

			//enable no parity and 8 byte word size
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
			options.c_cflag &= ~CSIZE;
			options.c_cflag |= CS8;

			//raw input mode
			options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

			//raw output mode
			options.c_oflag &= ~OPOST;

			//actually set the options
			tcsetattr(serial, TCSANOW, &options);

			os::checkError(__LINE__, __FILE__, "Error setting serial attributes for " + std::string(port));
#endif

			connected = true;
		}

		void Serial::destroy() {
			os::clearError();

			if (!connected) {
				MACE__THROW(AssertionFailed, "Can't destroy an unconnected Serial object!");
			}

#ifdef MACE_WINAPI
			CloseHandle(serial);
#elif defined(MACE_POSIX)
			close(serial);
#endif
			os::checkError(__LINE__, __FILE__, "Error closing serial port");

			connected = false;

		}

		int Serial::read(char * buffer, Size bufferSize) {
			os::clearError();

			if (!isConnected()) {
				MACE__THROW(InitializationFailed, "Failed to read from serial: Not connected to serial port");
			}

			if (!isValid()) {
				destroy();
				MACE__THROW(FileNotFound, "Socket closed. Please recall init()");
			}

			const Size available = getAvailableCharacterAmount();

			//dont want to overflow the buffer
			Size toRead = available > bufferSize ? bufferSize : available;

			if (toRead == 0) {
				return 0;
			}

#ifdef MACE_WINAPI
			DWORD bytesRead;

			if (ReadFile(serial, buffer, toRead, &bytesRead, nullptr)) {
				return bytesRead;
			}

			if (bytesRead == 0 && toRead > 0) {
				throw BadFileError("Failed to read from serial stream");
			}

			os::checkError(__LINE__, __FILE__, "Error reading from serial port");

			return bytesRead;
#elif defined(MACE_POSIX)
			ssize_t bytesRead = ::read(serial, buffer, toRead);

			os::checkError(__LINE__, __FILE__, "Error reading from serial port");

			return bytesRead;
#endif
		}

		void Serial::write(const char * buffer, const Size bufferSize) {
			os::clearError();

			if (!isConnected()) {
				MACE__THROW(InitializationFailed, "Failed to write to serial: Not connected to serial port");
			}

			if (!isValid()) {
				destroy();
				MACE__THROW(FileNotFound, "Socket closed. Please recall init()");
			}

			if (bufferSize == 0) {
				return;
			}

#ifdef MACE_WINAPI
			DWORD bytesSent;

			if (!WriteFile(serial, buffer, bufferSize, &bytesSent, 0)) {
				os::checkError(__LINE__, __FILE__, "Failed to write to serial port");
			}

			if (bytesSent == 0 && bufferSize > 0) {
				throw BadFileError("Unable to write to serial port");
			}

			return;
#elif defined(MACE_POSIX)
			ssize_t bytesWritten = ::write(serial, buffer, bufferSize);

			if (bytesWritten <= 0) {
				os::checkError(__LINE__, __FILE__, "Failed to write to serial port");
				return;
			}
#endif
		}

		void Serial::write(const std::string & port) {
			this->write(port.c_str(), static_cast<Size>(port.size()));
		}

		void Serial::flush() {
			os::clearError();

			if (!isConnected()) {
				MACE__THROW(InitializationFailed, "Unable to flush serial stream - it is not connected");
			}

			if (!isValid()) {
				destroy();
				MACE__THROW(FileNotFound, "Socket closed. Please recall init()");
			}

#ifdef MACE_WINAPI
			FlushFileBuffers(serial);

#elif defined(MACE_POSIX)
			tcflush(serial, TCIOFLUSH);
#endif

			os::checkError(__LINE__, __FILE__, "Failed to flush serial buffer");
		}

		Size Serial::getAvailableCharacterAmount() const {
			os::clearError();

			if (!isConnected()) {
				MACE__THROW(InitializationFailed, "Unable to read available characters from serial port - it is not connected");
			}

			if (!isValid()) {
				MACE__THROW(FileNotFound, "Socket closed. Please recall init()");
			}

#ifdef MACE_WINAPI
			DWORD errors;
			COMSTAT status;

			ClearCommError(serial, &errors, &status);

			os::checkError(__LINE__, __FILE__, "Failed to retrieve available character amount with error code: ClearCommError returned " + std::to_string(errors));

			return status.cbInQue;
#elif defined(MACE_POSIX)
			int bytes;

			ioctl(serial, FIONREAD, &bytes);

			os::checkError(__LINE__, __FILE__, "Failed to retrieve available character amount");

			return static_cast<Size>(bytes);
#endif
		}

		bool Serial::hasAvailable() const {
			return getAvailableCharacterAmount() > 0;
		}

		bool Serial::isConnected() const {
			return connected;
		}
		bool Serial::isValid() const {
			os::clearError();

#ifdef MACE_WINDOWS
			if (serial == INVALID_HANDLE_VALUE) {
				return false;
			}

			DCB commState;

			//cheatey way to check if it exists by seeing if this errors
			if (!GetCommState(serial, &commState)) {
				//get rid of the flag
				const DWORD lastError = GetLastError();

				return false;
			} else {
				return true;
			}
#elif defined(MACE_POSIX)
			const bool valid = fcntl(serial, F_GETFD) != -1 || errno != EBADF;;
			errno = 0;
			return valid;
#endif
		}
	}//os
}//mc
