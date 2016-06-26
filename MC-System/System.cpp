#include <MC-System/System.h>

typedef unsigned char byte;

namespace mc {
	void System::init() {
		SDL_Init(SDL_INIT_EVERYTHING);
	}

	void System::terminate() {
		SDL_Quit();
	}

	void System::pollevents() {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
				SDL_SetWindowData(SDL_GetWindowFromID(e.window.windowID), "open", 0);
			}
		}
	}
}