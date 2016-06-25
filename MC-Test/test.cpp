#include <MC-Window/Window.h>

using namespace mc;

int main() {
	System::init();

	Window window(800, 600, "MACE Window test!", false);
	
	while (!window.isCloseRequested()) {
		System::pollevents();
	}

	window.destroy();

	System::terminate();

	system("PAUSE");

	return 0;
}