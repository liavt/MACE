#include <MC-System/Exceptions.h>
#include <SDL/SDL.h>
#include <iostream>
#include <string>
#include <MC-System/System.h>

namespace mc {
	void Exception::handleException(const std::exception & e)
	{
		std::cerr << e.what()<<std::endl;
		System::requestStop();
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception occured", e.what(), nullptr);
	}
}