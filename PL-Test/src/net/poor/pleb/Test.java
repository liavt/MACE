package net.poor.pleb;

import net.pleb.audio.PLAudioManager;
import net.pleb.audio.PLSound;
import net.pleb.graphics.context.PLGraphicsContext;
import net.pleb.graphics.context.PLGraphicsContextGL;
import net.pleb.graphics.renderer.PLRenderer;
import net.pleb.graphics.renderer.PLRendererGL;
import net.pleb.network.tcp.PLTcpClient;
import net.pleb.network.tcp.PLTcpClientCallback;
import net.pleb.network.tcp.PLTcpServer;
import net.pleb.network.tcp.PLTcpServerCallback;
import net.pleb.system.PLColor;
import net.pleb.system.PLEngine;
import net.pleb.window.PLInput;
import net.pleb.window.PLWindow;

public class Test implements PLTcpClientCallback, PLTcpServerCallback{
	public static void main(String[] args) {
		new Test();
	}
	
	PLTcpClient client;
	PLTcpServer server;
	
	public Test() {
		PLEngine.init();//Initialize the engine
		PLAudioManager.init();//Initialize audio manager
		PLSound sound = new PLSound("/audio/Sound.wav");//Open Sound.wav file and load it into OpenAL
		
		server = new PLTcpServer(5000, this);//Create a tcp server on port 5000 with the 'this' object as a callback
		client = new PLTcpClient("localhost", 5000, this);//Create a tcp client that connects to a server at "localhost" on port 5000 with the 'this' object as a callback
		
		server.start();//Start the server (SERVER MUST BE STARTED FIRST!)
		client.start();//Start the client
		
		new Thread(() -> {
			boolean run = true;
			while(run) {
				try {
					Thread.sleep(1000);//Wait on second (1000 milliseconds)
				} catch (Exception e) {
					e.printStackTrace();
				}
				client.sendData("PING");//Send "PING" to server
			}
		}).start();//Create a thread that sends "PING" to the server from the client
		
		PLWindow window = new PLWindow(-1, -1, 800, 600, "Test", true);//Create a window with xOff:-1(centered), yOff:-1(centered), width:800 , height:800, title:"Test", and resizeable:true
		PLGraphicsContext context = new PLGraphicsContextGL(true).create(window);//Create a an OpenGL Graphics interface
		PLRenderer renderer = new PLRendererGL();//Create an OpenGL renderer
		
		while(window.isOpen()) {//while the window is open
			renderer.clear(PLColor.MAGENTA);//Clear the window and set the clear color to magenta
			PLEngine.pollEvents();//Poll all events
			context.swapBuffers(window);//Swap the graphics buffers
			
			if(PLInput.isKeyPressed(PLInput.KEY_SPACE)) {
				sound.play();  
			}
		}
		
		PLAudioManager.stopAll();//Stop all sounds
		PLEngine.exit(0);//Terminate engine
	}
	
	public void receivedData(int id, String data) {//Server received data from client with ID 'id'
		System.out.println("SERVER>> Client " + id + " sent: " + data);
		if(data.equals("PING")) {
			server.get(0).sendData("PONG");//Send the message "PONG" to client 0
		}
	}

	public void connected(int id) {//Server connected to client with ID 'id'
		System.out.println("SERVER>> Client " + id + " connected!");
	}

	public void connected() {//Client connected to server
		System.out.println("CLIENT>> Connected to server!");
	}

	public void receivedData(String data) {//Client received data from server
		System.out.println("CLIENT>> Server sent: " + data);
	}
}
