package net.pleb.graphics.context;

import net.pleb.window.PLWindow;

public abstract class PLGraphicsContext {
	public abstract PLGraphicsContext create(PLWindow window);
	public abstract void swapBuffers(PLWindow window);
}
