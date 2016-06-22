package net.pleb.network.udp;

import java.io.IOException;
import java.net.BindException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.HashMap;
import java.util.Map;

public class PLUdpServer implements Runnable{
	
	public class PLUdpServerClient{
		InetAddress address;
		int port;
		int num;
		boolean running = false;
		PLUdpServer server;
		
		public PLUdpServerClient(PLUdpServer server, InetAddress address, int port, int num) {
			this.address = address;
			this.port = port;
			this.num = num;
			this.server = server;
		}
		
		public void recivedData(byte[] data) {
			new Thread(() -> {
				server.callback.receivedData(num, data);
			}).start();
		}
		
		public void sendData(byte[] data) {
			if(data.length > server.packetSize) {
				System.err.println("Message longer than server max: " + data.length + " bytes");
			}
			
			DatagramPacket packet = new DatagramPacket(data, data.length, address, port);
			try {
				server.socket.send(packet);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
	
	public volatile Map<Integer, PLUdpServerClient> clients = new HashMap<Integer, PLUdpServerClient>();
	volatile Map<String, Integer> clientIDs = new HashMap<String, Integer>();
	public PLUdpServerCallback callback;
	Thread thread;
	int port;
	boolean running = false;
	DatagramSocket socket;
	public int packetSize;
	volatile int clientNum = 0;
	
	public PLUdpServer(int port, PLUdpServerCallback callback) {
		this(port, callback, 1024);
	}
	
	public PLUdpServer(int port, PLUdpServerCallback callback, int packetSize) {
		this.port = port;
		this.thread = new Thread(this);
		this.packetSize = packetSize;
		this.callback = callback;
	}
	
	public PLUdpServerClient get(int id) {
		return clients.get(id);
	}
	
	public PLUdpServerClient get(String address) {
		return clients.get(clientIDs.get(address));
	}
	
	public void start() {
		running = true;
		thread.setPriority(Thread.MAX_PRIORITY);
		thread.start();
	}
	
	public synchronized int getId() {
		clientNum++;
		return clientNum-1;
	}
	
	public void proccesMessage(DatagramPacket packet) {
		InetAddress address = packet.getAddress();
		String rawAddress = address.getHostAddress() + ":" + packet.getPort();
		PLUdpServerClient client = clients.get(clientIDs.get(rawAddress));
		if(client != null) {
			client.recivedData(packet.getData());
		} else {
			int idTemp = getId();
			PLUdpServerClient temp = new PLUdpServerClient(this, address, packet.getPort(), idTemp);
			clients.put(idTemp, temp);
			clientIDs.put(rawAddress, idTemp);
			temp.recivedData(packet.getData());
		}	
	}
	
	public void run() {
		try {
			socket = new DatagramSocket(port);
		} catch(BindException e) {
			System.err.println("Socket already boud!");
			System.exit(-1);
		} catch (IOException e) {
			e.printStackTrace();
		}
		while (running) {
			try {
				byte[] data = new byte[packetSize];
				DatagramPacket packet = new DatagramPacket(data, packetSize);
				socket.receive(packet);
				new Thread(() -> {
					proccesMessage(packet);
				}).start();
			}catch (Exception e) {
				e.printStackTrace();
			}
		}
	}
}
