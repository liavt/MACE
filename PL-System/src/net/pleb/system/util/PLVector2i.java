package net.pleb.system.util;

public class PLVector2i implements java.io.Serializable, Cloneable {
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	@Override
	public Object clone() throws CloneNotSupportedException {
		return new PLVector2i(x, y);
	}

	public int x = 0, y = 0;

	public int getX() {
		return x;
	}

	public void setX(int x) {
		this.x = x;
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
		result = prime * result + x;
		result = prime * result + y;
		return result;
	}

	@Override
	public String toString() {
		return "Vector2i[" + x + ", " + y + "]";
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
		PLVector2i other = (PLVector2i) obj;
		if (x != other.x) {
			return false;
		}
		if (y != other.y) {
			return false;
		}
		return true;
	}

	public int getY() {
		return y;
	}

	public void setY(int y) {
		this.y = y;
	}

	public PLVector2i(int x, int y) {
		this.x = x;
		this.y = y;
	}

	public PLVector2i() {
		
	}
}
