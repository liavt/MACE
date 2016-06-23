package net.pleb.graphics.renderer;

import static org.lwjgl.glfw.GLFW.*;
import static org.lwjgl.opengl.GL11.*;

import org.lwjgl.opengl.GL;

import net.pleb.graphics.PLShape;
import net.pleb.system.util.PLColor;
import net.pleb.window.PLWindow;

public class PLRendererGL extends PLRenderer {
	
	private PLWindow window;
	
	public PLRendererGL(PLWindow window, boolean vSync) {
		this.window = window;
		
		glfwMakeContextCurrent(window.getGLFWWindow());
		if(vSync) {
			glfwSwapInterval(1);
		} else {
			glfwSwapInterval(0);
		}
		
		GL.createCapabilities();
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, r, b, t, n, f);
	}

	@Override
	public void render(PLShape shape) {
		
	}

	@Override
	public void clear(PLColor color) {
		glClearColor(color.getRedAsFloat(), color.getGreenAsFloat(), color.getBlueAsFloat(), color.getAlphaAsFloat());
		glClear(GL_COLOR_BUFFER_BIT);
	}

	@Override
	public void swapBuffers() {
		glfwSwapBuffers(window.getGLFWWindow());
	}

}
