#include <MC-Graphics/GL/RendererGL11.h>

namespace mc {
	RendererGL11::RendererGL11(Window* window) : RendererGL(window) {
		
	}

	void RendererGL11::render() {
		glClearColor(1, 0, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);
	}
}