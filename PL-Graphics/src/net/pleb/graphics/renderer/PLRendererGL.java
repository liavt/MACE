package net.pleb.graphics.renderer;

import static org.lwjgl.opengl.GL11.*;

import net.pleb.graphics.PLShape;
import net.pleb.system.util.PLColor;
import net.pleb.window.PLWindow;

public class PLRendererGL extends PLRenderer {

	@Override
	public void init(PLWindow window) {

	}

	@Override
	public void render(PLShape shape) {

	}

	@Override
	public void clear(PLColor color) {
		glClearColor(color.getRedAsFloat(), color.getGreenAsFloat(), color.getBlueAsFloat(), color.getAlphaAsFloat());
		glClear(GL_COLOR_BUFFER_BIT);
	}

}
