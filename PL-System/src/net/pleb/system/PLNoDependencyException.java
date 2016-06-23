package net.pleb.system;

/**
 * Thrown when a {@link PLModule module's} dependency is not found
 * 
 * @author Liav
 * @see PLEngine#registerModule(PLModule)
 * @see PLModule#getDependencies()
 */
public class PLNoDependencyException extends RuntimeException {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public PLNoDependencyException() {
	}

	public PLNoDependencyException(String message, Throwable cause, boolean enableSuppression,
			boolean writableStackTrace) {
		super(message, cause, enableSuppression, writableStackTrace);
	}

	public PLNoDependencyException(String message, Throwable cause) {
		super(message, cause);
	}

	public PLNoDependencyException(String message) {
		super(message);
	}

	public PLNoDependencyException(Throwable cause) {
		super(cause);
	}

}
