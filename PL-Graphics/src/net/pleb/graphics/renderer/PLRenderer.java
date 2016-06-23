package net.pleb.graphics.renderer;

import net.pleb.graphics.PLShape;
import net.pleb.system.util.PLColor;

public abstract class PLRenderer {
	public abstract void clear(PLColor color);
	public abstract void render(PLShape shape);
	public abstract void swapBuffers();
}
