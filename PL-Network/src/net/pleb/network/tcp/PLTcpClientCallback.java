package net.pleb.network.tcp;

public interface PLTcpClientCallback {
	public void connected();
	public void receivedData(String data);
}
