package net.poor.pleb;

import net.pleb.system.PLEngine;
import net.pleb.window.PLInput;
import net.pleb.window.PLWindow;

public class Test {
	public static void main(String[] args) {
		PLEngine.init();
		
		PLWindow window = new PLWindow(-1, -1, 800, 600, "Engine", false);
		
		while(window.isOpen()) {
			System.out.println(PLInput.isKeyDown(PLInput.KEY_W));
			PLEngine.pollEvents();
		}
		
		PLEngine.terminate();
	}
}
