#include <MC-System/Exceptions.h>
#include <iostream>
#include <string>
#include <MC-System/System.h>

namespace mc {
	void Exception::handleException(const std::exception & e)
	{
		System::requestStop();
		std::cerr << "Error occured: "<<e.what()<<std::endl;
		throw e;
	}
}