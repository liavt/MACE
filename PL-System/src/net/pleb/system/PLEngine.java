package net.pleb.system;

import static org.lwjgl.glfw.GLFW.*;

import java.util.ArrayList;

public class PLEngine {
	private final static ArrayList<PLModule> MODULES = new ArrayList<>();

	public static void registerModule(PLModule module) {
		for (final String s : module.getDependencies()) {
			if (!doesModuleExist(s)) {
				throw new PLNoDependencyException(
						"Module " + s + ", needed by " + module.getName() + ", has not been registered!");
			}
		}

		MODULES.add(module);
	}

	public static boolean doesModuleExist(String name) {
		return getModuleByName(name) != null;
	}

	public static PLModule getModuleByName(String s) {
		for (final PLModule m : MODULES) {
			if (m.getName().equals(s)) {
				return m;
			}
		}
		return null;
	}

	public static void init() {
		if (!glfwInit()) {
			throw new RuntimeException("Could not initialize GLFW!");
		}

		for (final PLModule m : MODULES) {
			m.init();
		}
	}

	public static void tick() {
		glfwPollEvents();

		for (final PLModule m : MODULES) {
			m.tick();
		}
	}

	public static void destroy() {
		glfwTerminate();

		for (final PLModule m : MODULES) {
			m.destroy();
		}
	}

	public static void exit(int status) {
		destroy();
		System.exit(status);
	}
}
