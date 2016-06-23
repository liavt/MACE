package net.pleb.system.util;

import java.awt.Color;
import java.io.Serializable;

public class PLColor implements java.lang.Cloneable, Serializable {
	private static final long serialVersionUID = 1L;

	private int r = 255, g = 255, b = 255, a = 255;

	public static final PLColor RED = new PLColor(255, 0, 0), GREEN = new PLColor(0, 255, 0),
			DARK_BLUE = new PLColor(0, 0, 255), CYAN = new PLColor(0, 255, 255), LIGHT_BLUE = new PLColor(50, 200, 255),
			DARK_RED = new PLColor(150, 0, 0), DARK_GREEN = new PLColor(0, 150, 0), PURPLE = new PLColor(100, 0, 255),
			MAGENTA = new PLColor(255, 0, 255), WHITE = new PLColor(255, 255, 255), BLACK = new PLColor(0, 0, 0),
			YELLOW = new PLColor(255, 255, 0), DARK_GREY = new PLColor(100, 100, 100),
			LIGHT_GREY = new PLColor(175, 175, 175), DARK_ORANGE = new PLColor(255, 100, 0),
			LIGHT_ORANGE = new PLColor(255, 150, 0);

	public PLColor darker() {
		return new PLColor((int) (r * 0.9f), (int) (g * 0.9f), (int) (b * 0.9f));
	}

	public PLColor brighter() {
		return new PLColor((int) (r * 1.1f), (int) (g * 1.1f), (int) (b * 1.1f));
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "PLColor [r=" + r + ", g=" + g + ", b=" + b + ", a=" + a + "]";
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#hashCode()
	 */
	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + a;
		result = prime * result + b;
		result = prime * result + g;
		result = prime * result + r;
		return result;
	}

	public int getRGB() {
		return new Color(r, g, b, a).getRGB();
	}

	public String getHexcode() {
		return String.format("#%02x%02x%02x", r, g, b);
	}

	@Override
	public Object clone() {
		final int r = this.r, g = this.g, b = this.b, a = this.a;
		return new PLColor(r, g, b, a);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#equals(java.lang.Object)
	 */
	@Override
	public boolean equals(Object obj) {
		if (this == obj) {
			return true;
		}
		if (obj == null) {
			return false;
		}
		if (getClass() != obj.getClass()) {
			return false;
		}
		PLColor other = (PLColor) obj;
		if (a != other.a) {
			return false;
		}
		if (b != other.b) {
			return false;
		}
		if (g != other.g) {
			return false;
		}
		if (r != other.r) {
			return false;
		}
		return true;
	}

	public static PLColor fromHex(String hex) {
		if (hex.startsWith("#")) {
			return fromHex(Integer.parseInt(hex.substring(1), 16));
		} else if (hex.startsWith("0x")) {
			return fromHex(Integer.parseInt(hex.substring(2), 16));
		} else if (hex.startsWith("#")) {
			return fromHex(Integer.parseInt(hex.substring(1), 16));
		}
		return fromHex(Integer.parseInt(hex, 16));
	}

	public static PLColor fromHex(int hex) {
		final int r = (hex & 0xFF0000) >> 16;
		final int g = (hex & 0xFF00) >> 8;
		final int b = (hex & 0xFF);
		return new PLColor(r, g, b);
	}

	public PLColor(int r, int g, int b, int a) {
		this.r = r;
		this.g = g;
		this.b = b;
		this.a = a;
	}

	public PLColor(int r, int g, int b) {
		this(r, g, b, 255);
	}

	public PLColor(float r, float g, float b, float a) {
		this((int) (r * 255.0f), (int) (g * 255.0f), (int) (b * 255.0f), (int) (a * 255.0f));
	}

	public PLColor(float r, float g, float b) {
		this(r, g, b, 1);
	}

	public PLColor() {
		this(255, 255, 255, 255);
	}

	public PLColor(PLColor c) {
		this(c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha());
	}

	public PLColor(int[] color) {
		this(color[0], color[1], color[2], color[3]);
	}

	/**
	 * @return the r
	 */
	public int getRed() {
		return r;
	}

	/**
	 * @param r
	 *            the r to set
	 */
	public void setRed(int r) {
		this.r = r;
	}

	/**
	 * @return the g
	 */
	public int getGreen() {
		return g;
	}

	/**
	 * @param g
	 *            the g to set
	 */
	public void setGreen(int g) {
		this.g = g;
	}

	/**
	 * @return the b
	 */
	public int getBlue() {
		return b;
	}

	/**
	 * @param b
	 *            the b to set
	 */
	public void setBlue(int b) {
		this.b = b;
	}

	/**
	 * @return the a
	 */
	public int getAlpha() {
		return a;
	}

	/**
	 * @param a
	 *            the a to set
	 */
	public void setAlpha(int a) {
		this.a = a;
	}

	public int toByte() {
		return ((a)) << 24 | (g) << 16 | ((b)) << 8 | ((r));
	}

	public float getAlphaAsFloat() {
		return a / 255.0f;
	}

	public float getRedAsFloat() {
		return r / 255.0f;
	}

	public float getGreenAsFloat() {
		return g / 255.0f;
	}

	public float getBlueAsFloat() {
		return b / 255.0f;
	}

	public void setAlpha(float a) {
		this.a = (int) (a * 255.0f);
	}

	public void setRed(float r) {
		this.r = (int) (r * 255.0f);
	}

	public void setGreen(float g) {
		this.g = (int) (g * 255.0f);
	}

	public void setBlue(float b) {
		this.b = (int) (b * 255.0f);
	}
}
