package net.pleb.window;

import org.lwjgl.glfw.*;
 
import static org.lwjgl.glfw.GLFW.*;
import static org.lwjgl.system.MemoryUtil.*;

public class PLWindow {
	
	private long window;
	
	public PLWindow(int width, int height, String title) {
		this(width, height, title, false);
	}
	
	public PLWindow(int width, int height, String title, boolean resizeable) {
		this(-1, -1, width, height, title, resizeable);
	}
	
	public PLWindow(int x, int y, int width, int height, String title) {
		this(x, y, width, height, title, false);
	}
	
	public PLWindow(int x, int y, int width, int height, String title, boolean resizeable) {
		GLFWErrorCallback.createPrint(System.err).set();
		
		glfwDefaultWindowHints();
		
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		if(resizeable) {
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		} else {
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		}
		
		window = glfwCreateWindow(width, height, title, NULL, NULL);
		if(window == NULL) {
			throw new RuntimeException("Could ont create GLFW window!");
		}
		
		glfwSetKeyCallback(window, (window, key, scancode, action, mods) -> {
           PLInput.keyCallback(key, action);
        });
		
		GLFWVidMode vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		
		int XP = x;
		int YP = y;
		
		if(x == -1) {
			XP = (vidmode.width() - width) / 2;
		}
		if(y == -1) {
			YP = (vidmode.height() - height) / 2;
		}
		
		glfwSetWindowPos(window, XP, YP);
		
		glfwShowWindow(window);
	}
	
	public boolean isOpen() {
		return !glfwWindowShouldClose(window);
	}
	
	public long getGLFWWindow() {
		return window;
	}
}
