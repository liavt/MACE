package net.pleb.graphics.context;

import static org.lwjgl.glfw.GLFW.*;

import org.lwjgl.opengl.GL;

import net.pleb.window.PLWindow;

public class PLGraphicsContextGL extends PLGraphicsContext{
	
	boolean vSync;
	
	public PLGraphicsContextGL(boolean vSync) {
		this.vSync = vSync;
	}
	
	public PLGraphicsContext create(PLWindow window) {
		glfwMakeContextCurrent(window.getGLFWWindow());
		if(vSync) {
			glfwSwapInterval(1);
		} else {
			glfwSwapInterval(0);
		}
		
		GL.createCapabilities();
		
		return this;
	}
	
	public void swapBuffers(PLWindow window) {
		glfwSwapBuffers(window.getGLFWWindow());
	}

}
