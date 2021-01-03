package application;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class CommunicationThread implements Runnable {
	
	private String login, password, host;
	private int port;
	private char[] buffor = new char[1000];
	private Thread loginControllerThread;
	private List<String> loginContainer;
	private List<String> communicationContainer;
	private Type type;
	private List<Channel> subscribedChannels = Collections.synchronizedList(new ArrayList<>());
	
	public CommunicationThread() {}
	
	public CommunicationThread(String login, String password, String host, int port, Thread loginControllerThread, List<String> success, Type type, List<String> communicationContainer) {
		this.login = login;
		this.password = password;
		this.host = host;
		this.port = port;
		this.loginControllerThread = loginControllerThread;
		this.loginContainer = success;
		this.type = type;
		this.communicationContainer = communicationContainer;
	}
	
	@Override
	public void run() {
		//set connection
    	try (Socket clientSocket = new Socket(host, port)) {
    		OutputStream output = clientSocket.getOutputStream();
    		InputStream input = clientSocket.getInputStream();
    		BufferedReader reader = new BufferedReader(new InputStreamReader(input));
    		if(firstRequest(output, reader)) {
    			System.out.println("LoggedIn");    			
//	    		Thread ct = Thread.currentThread();
//	    		while(true) {
//	    			synchronized (ct) {
//	    				ct.wait(4000);
//					}    			
//	    			System.out.println(readMessagesFromServer(reader));
//	    			System.out.println("----------------------------");
//	    		}    			
    		}
    		System.out.println("End of CommunicationThread");
    	} catch (IOException ex) {
            System.out.println("Server not found: " + ex.getMessage());
//    	} catch (InterruptedException ex) {
//    		System.out.println("Thread interrupted");
		} catch (Exception ex) {
    		ex.printStackTrace();
			System.out.println("Unknown exception");
		}
	}	
	
	public int readMessagesFromServer(BufferedReader reader) throws IOException {
		if(reader.ready())
			return reader.read();
		else
			return -2;
	}		
	
	public boolean firstRequest(OutputStream output, BufferedReader reader) throws IOException {
		boolean accepted = false;
		int loginLength = login.length();
		int passwordLength = password.length();
		int totalLength = loginLength + passwordLength + 1;
		StringBuilder requestBuilder = new StringBuilder();
		requestBuilder.append(totalLength);
		if(type.equals(Type.REGISTRATION)) {
			requestBuilder.append(";0;");
		} else {
			requestBuilder.append(";1;");
		}
		requestBuilder.append(login);
		requestBuilder.append(";");
		requestBuilder.append(password);
		
		String request = requestBuilder.toString();
		output.write(request.getBytes());		
		reader.read(buffor, 0, 5);
		String response = String.valueOf(buffor);
		if(response.charAt(4) == '0') {
			loginContainer.remove(0);
			loginContainer.add("true");
			getUserChannels(reader);
			accepted = true;
		}
		synchronized (loginControllerThread) {
			loginControllerThread.notify();
		}
		return accepted;
	}

	private void getUserChannels(BufferedReader reader) throws IOException {
		while(reader.ready()) {
			reader.read(buffor, 0, 2);
			int length = 0;
			if(buffor[1] == ';') {
				length = Integer.valueOf(String.valueOf(buffor[0]));
				reader.read(buffor, 0, 2);
			} else {
				length = Integer.valueOf(String.valueOf(buffor[0])) * 10 + Integer.valueOf(String.valueOf(buffor[1]));
				reader.read(buffor, 0, 3);
			}
			reader.read(buffor, 0, length);
			String response = String.valueOf(buffor, 0, length);
			while(response.length() != length) {
				reader.read(buffor, 0, length - response.length());
				response += String.valueOf(buffor, 0, length);
			}
			subscribedChannels.add(new Channel(response.split(";")[0], response.split(";")[1]));
			communicationContainer.add(response.split(";")[1]);
		}
	}
	
	public List<Channel> getSubscribedChannels() {
		return subscribedChannels;
	}
}
