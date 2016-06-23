package net.pleb.graphics.renderer;

import net.pleb.graphics.PLShape;
import net.pleb.system.util.PLColor;
import net.pleb.window.PLWindow;

public abstract class PLRenderer {
	public abstract void init(PLWindow window);

	public abstract void clear(PLColor color);

	public abstract void render(PLShape shape);
}
