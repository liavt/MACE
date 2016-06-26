#include <MC-System/System.h>
#include <iostream>

int main(int argc, char** argv) {
	mc::System::init();

	SDL_Window* window = SDL_CreateWindow("Title!", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		800, 600, SDL_WINDOW_SHOWN);

	SDL_SetWindowData(window, "open", (void*) (1));

	bool quit = false;
	while (!quit) {
		mc::System::pollevents();

		int open = (int) SDL_GetWindowData(window, "open");
		//std::cout << "Open: " << open << "\n";
		if (open == 0) {
			quit = true;
		}

		SDL_Delay(100);
	}

	mc::System::terminate();

	return 0;
}