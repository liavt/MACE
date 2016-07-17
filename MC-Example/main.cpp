#include <MC-System/System.h>
#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <MC-Window/Window.h>

using namespace mc;

int main(int argc, char** argv) {
	System::init();

	win::Window window(800, 600, "Hello!");
	window.create();

	SDL_Window* sdlWin = window.getSDLWindow();
	SDL_GLContext context = SDL_GL_CreateContext(sdlWin);
	SDL_GL_MakeCurrent(sdlWin, context);

	glClearColor(1, 0, 1, 1);

	while (window.isOpen()) {
		System::update();
		glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(sdlWin);
	}

	SDL_GL_DeleteContext(context);
	window.destroy();

	System::terminate();

	return 0;
}