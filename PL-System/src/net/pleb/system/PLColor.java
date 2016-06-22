package net.pleb.system;

public class PLColor {
	public float r, g, b;
	
	public static PLColor BLACK   = new PLColor(0.0f, 0.0f, 0.0f);
	public static PLColor BLUE    = new PLColor(0.0f, 0.0f, 1.0f);
	public static PLColor GREEN   = new PLColor(0.0f, 1.0f, 0.0f);
	public static PLColor CYAN    = new PLColor(0.0f, 1.0f, 1.0f);
	public static PLColor RED     = new PLColor(1.0f, 0.0f, 0.0f);
	public static PLColor MAGENTA = new PLColor(1.0f, 0.0f, 1.0f);
	public static PLColor YELLOW  = new PLColor(1.0f, 1.0f, 0.0f);
	public static PLColor WHITE   = new PLColor(1.0f, 1.0f, 1.0f);
	
	public PLColor(int r, int g, int b) {
		this(r / 256.0f, g / 256.0f, b / 256.0f);
	}
	
	public PLColor(float r, float g, float b) {
		this.r = r;
		this.g = g;
		this.b = b;
	}
}
