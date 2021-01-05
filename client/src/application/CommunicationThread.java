package application;

import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;

import java.io.*;
import java.net.Socket;
import java.time.Duration;
import java.time.Instant;
import java.util.ArrayList;
import java.util.List;

public class CommunicationThread implements Runnable {

    private String login, password, host;
    private int port;
    private char[] buffor = new char[1000];
    private Thread applicationThread;
    private List<String> loginContainer;
    private List<String> communicationContainer;
    private Type typeAction;
    private ObservableList<String> posts;
    private ObservableList<String> numberOfNewMsgs;
    private ObservableList<Channel> channels = FXCollections.observableArrayList(new ArrayList<>());
    private String currentChannelID = "";
    private ObservableList<String> availableChannels = FXCollections.synchronizedObservableList(FXCollections.observableArrayList(new ArrayList<>()));
    private List<Waiting> waitingForConfirmation = new ArrayList<>();
    private long timeGap;
    private Instant now;

    public CommunicationThread() {
    }

    public CommunicationThread(String login, String password, String host, int port, Thread loginControllerThread, List<String> success, Type type, List<String> communicationContainer) {
        this.login = login;
        this.password = password;
        this.host = host;
        this.port = port;
        this.applicationThread = loginControllerThread;
        this.loginContainer = success;
        this.typeAction = type;
        this.communicationContainer = communicationContainer;
    }

    @Override
    public void run() {
        //set connection
        try (Socket clientSocket = new Socket(host, port)) {
            OutputStream output = clientSocket.getOutputStream();
            InputStream input = clientSocket.getInputStream();
            BufferedReader reader = new BufferedReader(new InputStreamReader(input));
            if (firstRequest(output, reader)) {
                System.out.println("LoggedIn");
                Thread ct = Thread.currentThread();
                mainLoop:
                while (true) {
                    synchronized (ct) {
                        ct.wait(50);
                    }
                    while (!communicationContainer.isEmpty()) {
                        synchronized (communicationContainer) {
                            String request = communicationContainer.get(0);
                            String type = request.split(";")[1];
                            System.out.println("Type: " + type);
                            communicationContainer.remove(0);
                            switch (type) {
                                case "2":
                                	String msg = request.split(";")[4];
                                    String channelID = currentChannelID;
                                    int length = msg.length() + channelID.length() + 1;
                                    request = length + ";" + type + ";" + channelID + ";" + msg;
                                    sendRequest(request, output, type);
                                    break;
                                case "3":
                                    sendRequest(request, output, type);
                                    break;
                                case "4":
                                    subscribeChannel(request, output, reader);
                                    break;
                                case "5":
                                	sendRequest(request, output, type);
                                    break;
                                case "8":
                                    currentChannelID = request.split(";")[2];
                                    sendRequest(request, output, type);
                                    break;
                                case "9":
                                	sendRequest(request, output, type);
                                    break;
                                case "logout":
                                    break mainLoop;
                                case "n":
                                    request = currentChannelID.length() + ";8;" + currentChannelID;
                                    sendRequest(request, output, "8");
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                    if (reader.ready()) {
                    	readFromServer(reader);                       
                    }
                    for(Waiting w : waitingForConfirmation) {                    
                    	now = Instant.now();
                    	timeGap = Math.abs(Duration.between(now, w.getCreation()).getSeconds());
                    	if(timeGap > 5) {
                    		System.out.println("Resend request: " + w.getRequest());
                    		w.setNewCreation(now);
                    		sendAgain(w.getRequest(), output);
                    	}
                    }
                }
            }
            System.out.println("End of CommunicationThread");
        } catch (IOException ex) {
            System.out.println("Server not found: " + ex.getMessage());
        } catch (Exception ex) {
            ex.printStackTrace();
            System.out.println("Unknown exception");
        }
    }

    private void sendAgain(String request, OutputStream output) throws IOException {
    	output.write(request.getBytes());
    }
    
    private void sendRequest(String request, OutputStream output, String type) throws IOException {
    	Waiting w = new Waiting(type, request);
        waitingForConfirmation.add(w);
    	output.write(request.getBytes()); 
    }

    private void subscribeChannel(String data, OutputStream output, BufferedReader reader) throws IOException, InterruptedException {
        String[] splitted = data.split(";");
        String channelName = splitted[3];
        String request = splitted[0] + ";4;" + splitted[2];

        Waiting w = new Waiting("4", request, channelName);
        waitingForConfirmation.add(w);
    	output.write(request.getBytes());    
    }
       
    private boolean firstRequest(OutputStream output, BufferedReader reader) throws IOException {
        boolean accepted = false;
        int loginLength = login.length();
        int passwordLength = password.length();
        int totalLength = loginLength + passwordLength + 1;
        StringBuilder requestBuilder = new StringBuilder();
        requestBuilder.append(totalLength);
        if (typeAction.equals(Type.REGISTRATION)) {
            requestBuilder.append(";0;");
        } else {
            requestBuilder.append(";1;");
        }
        requestBuilder.append(login);
        requestBuilder.append(";");
        requestBuilder.append(password);

        String request = requestBuilder.toString();
        output.write(request.getBytes());
        StringBuilder response = new StringBuilder();        
        for(int i = 0; i < 5; i++) {
        	reader.read(buffor, 0, 1);
        	response.append(buffor[0]);
        }                      
        if (response.charAt(4) == '0') {
        	synchronized (loginContainer) {
        		if(!loginContainer.isEmpty())
        			loginContainer.remove(0);
                loginContainer.add("true");
			}            
            if (typeAction.equals(Type.LOGIN))
                readFirstResponse(reader);
            accepted = true;
        } else {
        	synchronized (loginContainer) {
        		if(!loginContainer.isEmpty())
        			loginContainer.remove(0);
                loginContainer.add("false");
			}            
        }
        synchronized (applicationThread) {
            applicationThread.notify();
        }
        return accepted;
    }

    private void readFirstResponse(BufferedReader reader) throws IOException {
        channels.clear();
        String type;
        while (!reader.ready()) {}
        while (reader.ready()) {
            reader.read(buffor, 0, 2);
            int length = 0;
            if (buffor[1] == ';') {
                length = Integer.valueOf(String.valueOf(buffor[0]));
                reader.read(buffor, 0, 1);
                type = String.valueOf(buffor[0]);
            } else {
                length = Integer.valueOf(String.valueOf(buffor[0])) * 10 + Integer.valueOf(String.valueOf(buffor[1]));
                reader.read(buffor, 0, 2);
                type = String.valueOf(buffor[1]);
            }
            reader.read(buffor, 0, 1); //skip semi-colon
            switch (type) {
                case "6":
                    int i = reader.read(buffor, 0, length);
                    String request = String.valueOf(buffor).substring(0, i);
                    while (i != length) {
                        int tmp = reader.read(buffor, 0, length - i);
                        i += tmp;
                        request += String.valueOf(buffor).substring(0, tmp);
                    }
                    System.out.println("Notification for channel: " + String.valueOf(buffor[0]));
                    synchronized (channels) {
                        for (Channel c : channels) {
                            if (c.getId().equals(request)) {
                                c.increaseNumberOfMsgs();
                                break;
                            }
                        }
                    }
                    break;
                case "7":
                    reader.read(buffor, 0, length);
                    String response = String.valueOf(buffor, 0, length);
                    while (response.length() != length) {
                        reader.read(buffor, 0, length - response.length());
                        response += String.valueOf(buffor, 0, length);
                    }
                    if (!response.split(";")[0].equals("0"))
                        synchronized (channels) {
                            channels.add(new Channel(response));
                        }
                    break;
            }
        }
    }
    
    private void readFromServer(BufferedReader reader) throws IOException {
    	StringBuilder msgBuilder = new StringBuilder();
    	StringBuilder type = new StringBuilder();
    	int length = 0, counter = -1, semicolons = 0;
    	char c;
    	while(reader.ready() && counter < length) {
    		reader.read(buffor, 0, 1);
    		c = buffor[0];
    		if(c != ';' && semicolons < 1) {
    			length = 10 * length + Integer.valueOf(String.valueOf(c));
    		}
    		if(c == ';') {
    			semicolons += 1;
    		}
    		if(c == ';' && semicolons == 1) {
    			counter = 0;
    		}
    		if(semicolons == 1 && c != ';') {
    			type.append(c);
    		}
    		if((c != ';' && semicolons > 1) || (semicolons > 2 && counter < length)) {
    			msgBuilder.append(c);
    			counter++;
    		}
    	}
    	String msg = msgBuilder.toString();
    	System.out.println("Answer type " + type);
    	switch(type.toString()) {
    		case "2":
    			confirmPost(msg);
    			break;
    		case "3":
    			confirmNewChannel(msg);
    			break;
    		case "4":
    			confirmSubscribe(msg);
    			break;
    		case "5":
    			confirmUnsubscribe(msg);
    			break;	
    		case "6":
    			createNotification(msg);
    			break;
    		case "7":
    			readChannel(msg);
    			break;
    		case "8":
    			readPost(msg);
    			break;	
    		case "9":
    			readAvailableChannel(msg, reader);
    			break;
    	}
    }
    
    private void confirmPost(String code) {
    	String msg = "";
    	for(Waiting w : waitingForConfirmation) {
    		if(w.getType().equals("2")) {
    			waitingForConfirmation.remove(w);
    			msg = w.getRequest().split(";")[3];
    			break;
    		}
    	}
    	if(code.equals("0")) {    		
    		synchronized (posts) {
              posts.add(login + " said:");
              posts.add(msg);
    		}
    	} else {
    		//TODO: error msg
    	}
    }
    
    private void confirmNewChannel(String code) {
    	for(Waiting w : waitingForConfirmation) {
    		if(w.getType().equals("3")) {
    			waitingForConfirmation.remove(w);
    			break;
    		}
    	}
    	if(code.equals("1")) {    		
    		//TODO: error msg
    	}
    }
    
    private void confirmSubscribe(String code) {
    	String channelID = "";
    	String channelName = "";
    	String[] splitted;
    	for(Waiting w : waitingForConfirmation) {
    		if(w.getType().equals("4")) {
    			waitingForConfirmation.remove(w);
    			splitted = w.getRequest().split(";");
    			channelID = splitted[2];
    			channelName = w.getExtras();
    			break;
    		}
    	}
    	if(code.equals("0")) {    		
    		channels.add(new Channel(channelID, channelName));
    	} else {
    		//TODO: error msg
    	}
    }
    
    private void confirmUnsubscribe(String code) {
    	for(Waiting w : waitingForConfirmation) {
    		if(w.getType().equals("5")) {
    			waitingForConfirmation.remove(w);
    			break;
    		}
    	}
    	if(code.equals("1")) {    		
    		System.out.println("Error while unsubscribing");
    	}
    }
    
    private void createNotification(String channelID) {
    	for (Channel c : channels) {
            if (c.getId().equals(channelID)) {
                c.increaseNumberOfMsgs();
                break;
            }
        }
        if (channelID.equals(currentChannelID)) {
            for (Channel c : channels) {
                if (c.getId().equals(channelID)) {
                    synchronized (numberOfNewMsgs) {
                        numberOfNewMsgs.remove(0);
                        numberOfNewMsgs.add(String.valueOf(c.getNumberOfNewMsgs()));
                    }
                    break;
                }
            }
        }
    }
    
    private void readChannel(String msg) {
    	String[] splitted = msg.split(";");
    	String channelID = splitted[0];
    	String channelName = splitted[1];    	
    	synchronized (channels) {
            channels.add(new Channel(channelID, channelName));
        }
    }

    private void readPost(String msg) {
    	for(Waiting w : waitingForConfirmation) {
    		if(w.getType().equals("8")) {
    			waitingForConfirmation.remove(w);
    			break;
    		}
    	}
    	String[] splitted = msg.split(";");
    	String author = splitted[1];
    	String content = splitted[2];
    	synchronized (posts) {
            Platform.runLater(() -> {
    		posts.add(author + " said:");
    		posts.add(content);
            });
		}
    	synchronized (channels) {
            for (Channel c : channels) {
                if (c.getId().equals(currentChannelID)) {
                    c.clearNumberOfNewMsgs();
                    break;
                }
            }
        }
        synchronized (numberOfNewMsgs) {
            numberOfNewMsgs.remove(0);
            numberOfNewMsgs.add("0");
        }
    }

    private void readAvailableChannel(String msg, BufferedReader reader) throws IOException {
    	List<String> tmp = new ArrayList<>();
    	for(Waiting w : waitingForConfirmation) {
    		if(w.getType().equals("9")) {
    			waitingForConfirmation.remove(w);
    			break;
    		}
    	}
    	String[] splitted = msg.split(";");
    	String channelID = splitted[0];
    	if(channelID.equals("0"))
    		return;
    	String channelName = splitted[1];
    	boolean subscribed = false;
        for (Channel c : channels) {
            if (c.getName().equals(channelName)) {
                subscribed = true;
            }
        }
        if(!subscribed) {
        	tmp.add(msg);
        }
        readRestAvailableChannels(tmp, reader);
    }
    
    private void readRestAvailableChannels(List<String> tmp, BufferedReader reader) throws IOException {    	
    	while (!reader.ready()) {}
        while (reader.ready()) {
        	reader.read(buffor, 0, 2);
        	int length = 0;
        	if (buffor[1] == ';') {
        		length = Integer.valueOf(String.valueOf(buffor[0]));
        		reader.read(buffor, 0, 1);
        	} else {
        		length = Integer.valueOf(String.valueOf(buffor[0])) * 10 + Integer.valueOf(String.valueOf(buffor[1]));
        		reader.read(buffor, 0, 2);
        	}
        	reader.read(buffor, 0, 1); //skip semi-colon
        	reader.read(buffor, 0, length);
        	String response = String.valueOf(buffor, 0, length);
        	while (response.length() != length) {
        		reader.read(buffor, 0, length - response.length());
        		response += String.valueOf(buffor, 0, length);
        	}
        	if(response.split(";")[0].equals("0")) {
        		break;
        	}
        	boolean subscribed = false;
        	for (Channel c : channels) {
        		if (c.getName().equals(response.split(";")[1])) {
        			subscribed = true;
        		}
        	}
        	if (!subscribed) {
        		tmp.add(response);
        	}	
        }
        if (!tmp.isEmpty()) {
        	synchronized (availableChannels) {
        		availableChannels.clear();
        		availableChannels.addAll(tmp);
        	}
        }
    }

    public void setObservables(ObservableList<String> posts, ObservableList<String> numberOfNewMsgs) {
        this.posts = posts;
        this.numberOfNewMsgs = numberOfNewMsgs;
    }

    public List<String> getPosts() {
        return posts;
    }

    public ObservableList<Channel> getChannels() {
        return channels;
    }

    public ObservableList<String> getAvailableChannels() {
        return availableChannels;
    }
}
