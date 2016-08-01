#include <MC-System/Exceptions.h>
#include <SDL/SDL.h>
#include <string>

namespace mc {
	void Exception::handleException(const std::exception & e)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception occured", e.what(), nullptr);
	}
}