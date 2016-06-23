package net.pleb.system.util;

public class PLMath {
	public static boolean isPrime(int n) {
		if (n % 2 == 0) {
			return false;
		}
		for (int i = 3; i * i <= n; i += 2) {
			if (n % i == 0) {
				return false;
			}
		}
		return true;
	}

	public static int rgbToSRGB(PLColor c) {
		return rgbToSRGB(c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha());
	}

	public static int rgbToSRGB(int r, int g, int b) {
		return rgbToSRGB(r, g, b, 255);
	}

	public static int rgbToSRGB(int r, int g, int b, int a) {
		return ((a)) << 24 | (g) << 16 | ((b)) << 8 | ((r));
	}

	/**
	 * Returns the closest multiple of 2 there is to n
	 * 
	 * @param n
	 * @return closest multiple of 2
	 */
	public static int log2(int n) {
		int v = n;
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		return v;
	}

	public static PLQuad multiplyPLQuad(PLQuad rect, float num) {
		return new PLQuad((int) (rect.x * num), (int) (rect.y * num), (int) (rect.width * num),
				(int) (rect.height * num));
	}

	public static PLQuad subtractPLQuad(PLQuad rect, int num) {
		return new PLQuad(rect.x - num, rect.y - num, rect.width - num, rect.height - num);
	}

	public static PLQuad subtractPLQuad(PLQuad rect, PLQuad rect2) {
		return new PLQuad(rect.x - rect2.x, rect.y - rect2.y, rect.width - rect2.width, rect.height - rect2.height);
	}

	public static PLColor floatToColor(PLQuadF v) {
		return new PLColor((int) v.x, (int) v.y, (int) v.width, (int) v.height);
	}

	public static Object[] reverseArray(Object[] o) {
		final Object[] out = o;
		for (int i = 0; i < out.length / 2; i++) {
			final Object temp = out[i];
			out[i] = out[out.length - i - 1];
			out[out.length - i - 1] = temp;
		}
		return out;
	}

	public static int rgbToByte(int pix) {
		return ((pix & 0xff000000) >> 24) << 24 | (pix & 0xff) << 16 | ((pix & 0xff00) >> 8) << 8
				| ((pix & 0xff0000) >> 16);
	}

	public static PLQuadF multiplyQuad(PLQuadF r, float f) {
		return new PLQuadF(r.x * f, r.y * f, r.width * f, r.height * f);
	}

	public static PLQuadF divideQuad(PLQuadF r, float f) {
		return new PLQuadF(r.x / f, r.y / f, r.width / f, r.height / f);
	}

	public static PLQuadF divideQuad(PLQuad r, int f) {
		return new PLQuadF(r.x / f, r.y / f, r.width / f, r.height / f);
	}
}
