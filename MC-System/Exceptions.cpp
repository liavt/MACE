#include <MC-System/Exceptions.h>
#include <iostream>
#include <string>
#include <MC-System/System.h>

namespace mc {
	void Exception::handleException(const std::exception & e)
	{
		std::cerr << e.what()<<std::endl;
		System::requestStop();
	}
}