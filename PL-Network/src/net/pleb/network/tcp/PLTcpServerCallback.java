package net.pleb.network.tcp;

public interface PLTcpServerCallback {
	public void receivedData(int id, String data);
	public void connected(int id);
}
