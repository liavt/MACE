package net.pleb.system.util;

public class PLPosition implements java.io.Serializable, Cloneable {
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	@Override
	public Object clone() throws CloneNotSupportedException {
		return new PLPosition(x, y);
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
		return "Position[ x: " + x + ", y: " + y + " ]";
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
		PLPosition other = (PLPosition) obj;
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

	public PLPosition(int x, int y) {
		this.x = x;
		this.y = y;
	}

	public PLPosition() {
	}
}
