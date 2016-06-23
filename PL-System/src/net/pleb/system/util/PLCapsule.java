package net.pleb.system.util;

/**
 * In a lambda expression, you must use final variables. This class stores an
 * int value, and that integer can be modified via methods, which allows for the
 * modification of an int in a lambda expression
 * 
 * @param <T> type of object to store
 * 
 *
 */
public class PLCapsule<T> implements java.io.Serializable {
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private T obj;

	/**
	 * Creates a new {@link IntCapsule} with a specified value
	 * 
	 * @param obj
	 *            starting value
	 **/
	public PLCapsule(T obj) {
		this.obj = obj;
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
		result = prime * result + ((obj == null) ? 0 : obj.hashCode());
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
		PLCapsule<?> other = (PLCapsule<?>) obj;
		if (this.obj == null) {
			if (other.obj != null) {
				return false;
			}
		} else if (!this.obj.equals(other.obj)) {
			return false;
		}
		return true;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "LambdaObject [obj=" + obj + "]";
	}

	/**
	 * @return the obj
	 */
	public T get() {
		return obj;
	}

	/**
	 * @param obj
	 *            the obj to set
	 */
	public void set(T obj) {
		this.obj = obj;
	}
}
