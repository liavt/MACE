package net.pleb.network.udp;

public interface PLUdpServerCallback {
	public void receivedData(int id, byte[] data);
}
