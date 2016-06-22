package net.poor.pleb;

import net.pleb.graphics.context.PLGraphicsContext;
import net.pleb.graphics.context.PLGraphicsContextGL;
import net.pleb.graphics.renderer.PLRenderer;
import net.pleb.graphics.renderer.PLRendererGL;
import net.pleb.system.PLColor;
import net.pleb.system.PLEngine;
import net.pleb.window.PLWindow;

public class Test {
	public static void main(String[] args) {
		PLEngine.init();
		
		PLWindow window = new PLWindow(800, 600, "Engine");
		PLGraphicsContext context = new PLGraphicsContextGL(true).create(window);
		PLRenderer renderer = new PLRendererGL();
		
		while(window.isOpen()) {
			renderer.clear(PLColor.MAGENTA);
			PLEngine.pollEvents();
			context.swapBuffers(window);
		}
		
		PLEngine.terminate();
	}
}
