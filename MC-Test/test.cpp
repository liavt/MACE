#include <MC-Graphics/RendererGL.h>
#include <MC-Window/Window.h>
#include <MC-Audio/SoundManager.h>
#include <MC-Audio/Sound.h>

using namespace mc;

int main() {
	System::init();

	Window window(800, 600, "MACE Window test!", false);
	Renderer* renderer = new RendererGL(&window);
	renderer->init();

	SoundManager::init();

	Sound sound("Sound.ogg");

	sound.play();

	glClearColor(1, 0, 1, 1);

	while (!window.isCloseRequested()) {
		glClear(GL_COLOR_BUFFER_BIT);

		if (Keyboard::isKeyPressed(GLFW_KEY_SPACE)) {
			std::cout << "Space pressed!\n";
		}

		if (Keyboard::isKeyDown(GLFW_KEY_SPACE)) {
			std::cout << "Space down!\n";
		}

		renderer->swapBuffers();

		System::pollevents();
	}

	window.destroy();

	SoundManager::destroy();

	System::terminate();

	return 0;
}