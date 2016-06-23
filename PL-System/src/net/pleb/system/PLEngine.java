package net.pleb.system;

import static org.lwjgl.glfw.GLFW.*;

public class PLEngine {
	
	public static void init() {
		if(!glfwInit()) {
			throw new RuntimeException("Could not initialize GLFW!");
		}
	}
	
	public static void pollEvents() {
		glfwPollEvents();
	}
	
	public static void terminate() {
		glfwTerminate();
	}
	
	public static void exit(int status) {
		terminate();
		System.exit(status);
	}
}
