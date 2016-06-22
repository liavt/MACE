package net.pleb.network.tcp;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.BindException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

public class PLTcpServer implements Runnable{
	public class PLTcpServerClient implements Runnable{
		Socket socket;
		Thread thread;
		PrintWriter out;
		BufferedReader in;
		int num;
		boolean running = false;
		Thread moveThread;
		PLTcpServer server;
		String tempInput = "";
		
		public PLTcpServerClient(PLTcpServer server, Socket socket, int num) {
			this.socket = socket;
			this.thread = new Thread(this);
			this.num = num;
			this.server = server;
		}
		
		public void start() {
			running = true;
			thread.start();
		}
		
		public void sendData(String data) {
			out.println(data);
		}
		
		public void run() {
			try {
				out = new PrintWriter(socket.getOutputStream(), true);
				in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
			} catch (IOException e) {
				e.printStackTrace();
			}
			
			server.callback.connected(num);
			
			while(server.running && running) {
				try {
					String input = in.readLine();
					tempInput = input;
					
					(new Thread(() -> {
						server.callback.receivedData(num, tempInput);
					})).start();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
	}
	
	public PLTcpServerCallback callback;
	volatile List<PLTcpServerClient> clients = new ArrayList<PLTcpServerClient>();
	ServerSocket server;
	Thread thread;
	int port;
	boolean running = false;
	
	public PLTcpServer(int port, PLTcpServerCallback callback) {
		this.port = port;
		this.thread = new Thread(this);
		this.callback = callback;
	}
	
	public void start() {
		running = true;
		thread.setPriority(Thread.MAX_PRIORITY-1);
		thread.start();
	}
	
	public PLTcpServerClient get(int id) {
		return clients.get(id);
	}
	
	public void run() {
		try {
			server = new ServerSocket(port);
		} catch(BindException e) {
			System.err.println("Socket already boud!");
			System.exit(-1);
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		try {
			Socket clientSocket = server.accept();
			PLTcpServerClient client = new PLTcpServerClient(this, clientSocket, clients.size());
			client.start();
			clients.add(client);
		}catch (IOException e) {
			e.printStackTrace();
		}
	}
}
