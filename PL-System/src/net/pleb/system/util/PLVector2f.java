package net.pleb.system.util;

public class PLVector2f implements java.lang.Cloneable, java.io.Serializable {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	@Override
	public Object clone() {
		return new PLVector2f(x, y);
	}

	public float x = 0, y = 0;

	public PLVector2f(float x, float y) {
		this.x = x;
		this.y = y;
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
		result = prime * result + Float.floatToIntBits(x);
		result = prime * result + Float.floatToIntBits(y);
		return result;
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
		PLVector2f other = (PLVector2f) obj;
		return !(Float.floatToIntBits(y) != Float.floatToIntBits(other.y)
				|| Float.floatToIntBits(x) != Float.floatToIntBits(other.x));
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "Vector2f [" + x + ", " + y + "]"; //$NON-NLS-1$ //$NON-NLS-2$ //$NON-NLS-3$
	}

	/**
	 * @return the x
	 */
	public float getX() {
		return x;
	}

	/**
	 * @param x
	 *            the x to set
	 */
	public void setX(float x) {
		this.x = x;
	}

	/**
	 * @return the y
	 */
	public float getY() {
		return y;
	}

	/**
	 * @param y
	 *            the y to set
	 */
	public void setY(float y) {
		this.y = y;
	}
}
