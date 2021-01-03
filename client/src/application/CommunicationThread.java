package application;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;

public class CommunicationThread implements Runnable {
	
	private String login, password, host;
	private int port;
	
	public CommunicationThread() {}
	
	public CommunicationThread(String login, String password, String host, int port) {
		this.login = login;
		this.password = password;
		this.host = host;
		this.port = port;
	}
	
	@Override
	public void run() {
		//set connection
    	try (Socket clientSocket = new Socket(host, port)) {
    		OutputStream output = clientSocket.getOutputStream();
    		InputStream input = clientSocket.getInputStream();
    		int loginLength = login.length();
    		int passwordLength = password.length();
    		int totalLength = loginLength + passwordLength + 1;
    		String request = totalLength + ";0;" + login + ";" + password;
    		System.out.println(request);
    		PrintWriter writer = new PrintWriter(output);
    		writer.write(request);
    		BufferedReader reader = new BufferedReader(new InputStreamReader(input));
    		int cos = reader.read();
    		System.out.println(cos);
    	} catch (IOException ex) {
            System.out.println("Server not found: " + ex.getMessage());
    	} catch (Exception e) {
			System.out.println("Unknown exception");
		}
		
	}	

}
