package application;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

import javafx.collections.ObservableList;

public class CommunicationThread implements Runnable {
	
	private String login, password, host;
	private int port;
	private char[] buffor = new char[1000];
	private Thread applicationThread;
	private List<String> loginContainer;
	private List<String> communicationContainer;
	private Type type;
	private ObservableList<String> posts;
	private List<Channel> channels = new ArrayList<>();
	
	public CommunicationThread() {}
	
	public CommunicationThread(String login, String password, String host, int port, Thread loginControllerThread, List<String> success, Type type, List<String> communicationContainer) {
		this.login = login;
		this.password = password;
		this.host = host;
		this.port = port;
		this.applicationThread = loginControllerThread;
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
	    		Thread ct = Thread.currentThread();
	    		mainLoop: while(true) {
	    			synchronized (ct) {
	    				ct.wait(2000);
					}    			
	    			while(!communicationContainer.isEmpty()) {
	    				String request = communicationContainer.get(0);
	    				String type = (request.split(";")[1]).substring(0, 1);
	    				System.out.println("Type: " + type);
	    				communicationContainer.remove(0);
	    				switch (type) {
	    				case "2":
	    					sendPost(request, output, reader);
	    					break;
						case "8":
							requestForPosts(request, output, reader);
							break;
						case "l":
							break mainLoop;
						default:
							break;
						}
	    			}
	    			if(reader.ready()) {
	    				int length = reader.read(buffor);
	    				String response = String.valueOf(buffor).substring(0, length);
	    			}
	    		}    			
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
	
	private int readMessagesFromServer(BufferedReader reader) throws IOException {
		if(reader.ready())
			return reader.read();
		else
			return -2;
	}		
	
	private boolean firstRequest(OutputStream output, BufferedReader reader) throws IOException {
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
		System.out.println(request);
		output.write(request.getBytes());		
		reader.read(buffor, 0, 5);
		String response = String.valueOf(buffor);
		if(response.charAt(4) == '0') {
			loginContainer.remove(0);
			loginContainer.add("true");
			readFirstResponse(reader);
			accepted = true;
		}
		synchronized (applicationThread) {
			applicationThread.notify();
		}
		return accepted;
	}

	private void readFirstResponse(BufferedReader reader) throws IOException {
		channels.clear();
		String type;
		while(!reader.ready()) {}
		while(reader.ready()) {
			reader.read(buffor, 0, 2);
			int length = 0;
			if(buffor[1] == ';') {
				length = Integer.valueOf(String.valueOf(buffor[0]));
				reader.read(buffor, 0, 1);
				type = String.valueOf(buffor[0]);
			} else {
				length = Integer.valueOf(String.valueOf(buffor[0])) * 10 + Integer.valueOf(String.valueOf(buffor[1]));
				reader.read(buffor, 0, 2);
				type = String.valueOf(buffor[1]);
			}
			reader.read(buffor, 0, 1); //skip semi-colon
			switch(type) {
				case "6":
					reader.read(buffor, 0, 1);				
					System.out.println("Notification for channel: " + String.valueOf(buffor[0]));
					break;
				case "7":
					reader.read(buffor, 0, length);
					String response = String.valueOf(buffor, 0, length);
					while(response.length() != length) {
						reader.read(buffor, 0, length - response.length());
						response += String.valueOf(buffor, 0, length);
					}
					System.out.println(response);
					channels.add(new Channel(response));
					communicationContainer.add(response);
					break;
			}
		}
	}

	private void sendPost(String post, OutputStream output, BufferedReader reader) throws IOException, InterruptedException {		
		String msg = post.split(";")[4];
		if(!msg.isEmpty()) {
			String channelName = post.split(";")[3];
			String channelID = "";
			for(Channel c : channels) {
				if(c.getName().equals(channelName)) {
					channelID = c.getId();
					break;
				}
			}
			int length = msg.length() + channelID.length() + 1;
			String request = length + ";2;" + channelID + ";" + msg; 
			output.write(request.getBytes());
			int counter = 0;
			System.out.println(request);
			while(!reader.ready() && counter < 20) {
				Thread.currentThread().sleep(100);
				counter++;
			}
			if(!reader.ready()) {
				//TODO resend
			} else {
				reader.read(buffor, 0, 5);
				synchronized (posts) {
					posts.add(login + " said:");
					posts.add(msg);
				}
			}
		}
	}
	
	@SuppressWarnings("static-access")
	private void requestForPosts(String request, OutputStream output, BufferedReader reader) throws IOException, InterruptedException {		
		output.write(request.getBytes());
		int counter = 0;
		while(!reader.ready() && counter < 20) {
			Thread.currentThread().sleep(100);
			counter++;
		}
		synchronized (posts) {	
			while(reader.ready()) {
				reader.read(buffor, 0, 2);
				int length = 0;
				if(buffor[1] == ';') {
					length = Integer.valueOf(String.valueOf(buffor[0]));
					reader.read(buffor, 0, 2);
				} else {
					length = Integer.valueOf(String.valueOf(buffor[0])) * 10 + Integer.valueOf(String.valueOf(buffor[1]));
					reader.read(buffor, 0, 1);
					if(buffor[0] == ';') {
						reader.read(buffor, 0, 2);
					} else {
						length *= 10;
						length += Integer.valueOf(String.valueOf(buffor[0]));
						reader.read(buffor, 0, 3);
					}
				}
				reader.read(buffor, 0, length);
				String response = String.valueOf(buffor, 0, length);
				while(response.length() != length) {
					reader.read(buffor, 0, length - response.length());
					response += String.valueOf(buffor, 0, length);
				}
				posts.add(response.split(";")[1]+ " said:");
				posts.add(response.split(";")[2]);
			}
			System.out.println("All messages read");
		}
	}
	
	public void setPosts(ObservableList<String> posts) {
		this.posts = posts;
	}
	
	public List<String> getPosts() {
		return posts;
	}		
}
