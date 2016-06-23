package net.pleb.system;

public interface PLModule {

	public String[] getDependencies();

	public void init();

	public void tick();

	public void destroy();

	public String getName();
}
