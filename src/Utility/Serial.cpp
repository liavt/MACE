/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Utility/Serial.h>
#include <MACE/Core/System.h>
#include <MACE/Core/Error.h>
#include <MACE/Utility/Math.h>

#ifdef MACE_WINAPI
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	undef WIN32_LEAN_AND_MEAN
#elif defined(MACE_POSIX)
#	include <sys/stat.h>
#	include <sys/ioctl.h>
#	include <fcntl.h>
#	include <termios.h>
#	include <unistd.h>
#endif

namespace mc {
	namespace os {
		Serial::Serial() : serial(nullptr) {}

		Serial::Serial(const char* port, const unsigned int baudRate) : Serial() {
			init(port, baudRate);
		}

		Serial::Serial(const std::string& port, const unsigned int baudRate) : Serial(port.c_str(), baudRate) {}

		Serial::~Serial() {
			if (isConnected()) {
				destroy();
			}
		}

		void Serial::init(const char* port, const unsigned int baudRate) {
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
					MACE__THROW(System, "Access denied accessing serial port at " + std::string(port));
				} else {
					MACE__THROW(FileNotFound, "Error opening " + std::string(port) + " with error from CreateFile " + std::to_string(lastError));
				}
			}

			os::checkError(__LINE__, __FILE__, "Error opening serial socket with CreateFile at " + std::string(port));

			DCB serialParams = {0};

			if (!GetCommState(serial, &serialParams)) {
				MACE__THROW(BadFile, "Error getting serial parameters from " + std::string(port));
			}

			serialParams.BaudRate = baudRate;
			serialParams.ByteSize = 8;
			serialParams.StopBits = ONESTOPBIT;
			serialParams.Parity = NOPARITY;
			serialParams.fDtrControl = DTR_CONTROL_ENABLE;

			if (!SetCommState(serial, &serialParams)) {
				MACE__THROW(BadFile, "Error setting serial parameters for " + std::string(port));
			}

			os::checkError(__LINE__, __FILE__, "Error setting serial communication status for " + std::string(port));

			COMMTIMEOUTS timeout = {0};
			timeout.ReadIntervalTimeout = 50;
			timeout.ReadTotalTimeoutConstant = 50;
			timeout.ReadTotalTimeoutMultiplier = 50;
			timeout.WriteTotalTimeoutConstant = 50;
			timeout.WriteTotalTimeoutMultiplier = 10;

			if (!SetCommTimeouts(serial, &timeout)) {
				MACE__THROW(BadFile, "Error setting serial timeout for " + std::string(port));
			}

			os::checkError(__LINE__, __FILE__, "Error setting serial communication timeouts for " + std::string(port));

			//we shall purge everything to flush any previous characters;
			if (!PurgeComm(serial, PURGE_RXCLEAR | PURGE_TXCLEAR)) {
				MACE__THROW(BadFile, "Error purging serial communications for " + std::string(port));
			};

			os::checkError(__LINE__, __FILE__, "Error purging previous messages on the serial communication line at " + std::string(port));
#elif defined(MACE_POSIX)
			//tell it to not block or own the process
			serial = open(port, O_RDWR | O_NOCTTY | O_NDELAY, S_IRWXU);

			os::checkError(__LINE__, __FILE__, "Error opening serial port at " + std::string(port));

			if (serial == -1) {
				MACE__THROW(FileNotFound, "Unable to open serial port " + std::string(port));
			}

			//we dont want to block reading - only return 0.
			fcntl(serial, F_SETFL, FNDELAY);

			os::checkError(__LINE__, __FILE__, "Error setting serial block settings for " + std::string(port));

			struct termios options;

			//get current options
			tcgetattr(serial, &options);

			os::checkError(__LINE__, __FILE__, "Error getting serial attributes for " + std::string(port));

			speed_t baud;
			switch (baudRate) {
			case 0:
				baud = B0;
				break;
			case 50:
				baud = B50;
				break;
			case 75:
				baud = B75;
				break;
			case 110:
				baud = B110;
				break;
			case 134:
				baud = B134;
				break;
			case 150:
				baud = B150;
				break;
			case 200:
				baud = B200;
				break;
			case 300:
				baud = B300;
				break;
			case 600:
				baud = B600;
				break;
			case 1200:
				baud = B1200;
				break;
			case 1800:
				baud = B1800;
				break;
			case 2400:
				baud = B2400;
				break;
			case 4800:
				baud = B4800;
				break;
			case 9600:
				MACE_FALLTHROUGH;
			default:
				baud = B9600;
				break;
			case 19200:
				baud = B19200;
				break;
			case 38400:
				baud = B38400;
				break;
			case 57600:
				baud = B57600;
				break;
			case 115200:
				baud = B115200;
				break;
			}

			cfsetispeed(&options, baud);
			cfsetospeed(&options, baud);

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

		int Serial::read(char* buffer, unsigned int bufferSize) {
			os::clearError();

			if (!isConnected()) {
				MACE__THROW(InitializationFailed, "Failed to read from serial: Not connected to serial port");
			}

			if (!isValid()) {
				destroy();
				MACE__THROW(FileNotFound, "Socket closed. Please recall init()");
			}

			const unsigned int available = getAvailableCharacterAmount();

			//dont want to overflow the buffer
			unsigned int toRead = math::min(bufferSize, available);

			if (toRead == 0) {
				return 0;
			}

#ifdef MACE_WINAPI
			DWORD bytesRead;

			if (ReadFile(serial, buffer, toRead, &bytesRead, nullptr)) {
				return bytesRead;
			}

			if (bytesRead == 0 && toRead > 0) {
				MACE__THROW(BadFile, "Failed to read from serial stream");
			}

			os::checkError(__LINE__, __FILE__, "Error reading from serial port");

			return bytesRead;
#elif defined(MACE_POSIX)
			ssize_t bytesRead = ::read(serial, buffer, toRead);

			os::checkError(__LINE__, __FILE__, "Error reading from serial port");

			return bytesRead;
#endif
		}

		void Serial::write(const char* buffer, const unsigned int bufferSize) {
			os::clearError();

			if (!isConnected()) {
				MACE__THROW(InitializationFailed, "Failed to write to serial: Not connected to serial port");
			}

			if (!isValid()) {
				destroy();
				MACE__THROW(FileNotFound, "Socket closed: recall init()");
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
				MACE__THROW(BadFile, "Unable to write to serial port");
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
			this->write(port.c_str(), static_cast<unsigned int>(port.size()));
		}

		void Serial::flush() {
			os::clearError();

			if (!isConnected()) {
				MACE__THROW(InitializationFailed, "Unable to flush serial stream: it is not connected");
			}

			if (!isValid()) {
				destroy();
				MACE__THROW(FileNotFound, "Socket closed: recall init()");
			}

#ifdef MACE_WINAPI
			FlushFileBuffers(serial);

#elif defined(MACE_POSIX)
			tcflush(serial, TCIOFLUSH);
#endif

			os::checkError(__LINE__, __FILE__, "Failed to flush serial buffer");
		}

		unsigned int Serial::getAvailableCharacterAmount() const {
			os::clearError();

			if (!isConnected()) {
				MACE__THROW(InitializationFailed, "Unable to read available characters from serial port - it is not connected");
			}

			if (!isValid()) {
				MACE__THROW(FileNotFound, "Socket closed: recall init()");
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
				//get rid of the dummy error we created
				const DWORD lastError = GetLastError();

				return false;
			} else {
				return true;
			}
#elif defined(MACE_POSIX)
			const bool valid = fcntl(serial, F_GETFD) != -1 || errno != EBADF;
			errno = 0;
			return valid;
#endif
		}
	}//os
}//mc
