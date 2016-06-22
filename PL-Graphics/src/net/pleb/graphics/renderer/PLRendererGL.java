package net.pleb.graphics.renderer;

import static org.lwjgl.opengl.GL11.*;

import net.pleb.graphics.PLShape;
import net.pleb.system.PLColor;
import net.pleb.window.PLWindow;

public class PLRendererGL extends PLRenderer{

	
	public void init(PLWindow window) {
		
	}

	public void render(PLShape shape) {
		
	}
	
	public void clear(PLColor color) {
		glClearColor(color.r, color.g, color.b, 1);
		glClear(GL_COLOR_BUFFER_BIT);
	}

}
