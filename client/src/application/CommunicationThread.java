package application;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.Socket;
import java.util.List;

public class CommunicationThread implements Runnable {
	
	private String login, password, host;
	private int port;
	private char[] buffor = new char[1000];
	private Thread loginControllerThread;
	private List<String> communicationContainer;
	private Type type;
	
	public CommunicationThread() {}
	
	public CommunicationThread(String login, String password, String host, int port, Thread loginControllerThread, List<String> success, Type type) {
		this.login = login;
		this.password = password;
		this.host = host;
		this.port = port;
		this.loginControllerThread = loginControllerThread;
		this.communicationContainer = success;
		this.type = type;
	}
	
	@Override
	public void run() {
		//set connection
    	try (Socket clientSocket = new Socket(host, port)) {
    		OutputStream output = clientSocket.getOutputStream();
    		InputStream input = clientSocket.getInputStream();
    		BufferedReader reader = new BufferedReader(new InputStreamReader(input));
    		firstRequest(output, reader);
    		Thread ct = Thread.currentThread();
//    		while(true) {
//    			synchronized (ct) {
//    				ct.wait(4000);
//				}    			
//    			System.out.println(readMessagesFromServer(reader));
//    			System.out.println("----------------------------");
//    		}
    		
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
		return reader.read();
	}
	
	public void firstRequest(OutputStream output, BufferedReader reader) throws IOException {
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
		requestBuilder.append("password");
		
		String request = requestBuilder.toString();
		output.write(request.getBytes());		
		reader.read(buffor);
		String response = String.valueOf(buffor);
		if(response.charAt(4) == '0') {
			communicationContainer.remove(0);
			communicationContainer.add("true");
		}
		synchronized (loginControllerThread) {
			loginControllerThread.notify();
		}
	}

}
