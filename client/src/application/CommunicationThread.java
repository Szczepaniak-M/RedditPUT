package application;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;

import java.io.*;
import java.net.Socket;
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
                        ct.wait(2000);
                    }
                    while (!communicationContainer.isEmpty()) {
                        synchronized (communicationContainer) {
                            String request = communicationContainer.get(0);
                            String type = request.split(";")[1];
                            System.out.println("Type: " + type);
                            communicationContainer.remove(0);
                            switch (type) {
                                case "2":
                                    sendPost(request, output, reader);
                                    break;
                                case "3":
                                    addChannel(request, output, reader);
                                    break;
                                case "4":
                                    subscribeChannel(request, output, reader);
                                    break;
                                case "5":
                                    unsubscribeChannel(request, output, reader);
                                    break;
                                case "8":
                                    currentChannelID = request.split(";")[2];
                                    requestForPosts(request, output, reader);
                                    break;
                                case "9":
                                    requestForAvailableChannels(request, output, reader);
                                    break;
                                case "logout":
                                    break mainLoop;
                                case "n":
                                    request = currentChannelID.length() + ";8;" + currentChannelID;
                                    requestForPosts(request, output, reader);
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                    if (reader.ready()) {
                        int length = reader.read(buffor);
                        String response = String.valueOf(buffor).substring(0, length);
                        String channelID = response.split(";")[2];
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
        reader.read(buffor, 0, 5);
        String response = String.valueOf(buffor);
        if (response.charAt(4) == '0') {
            loginContainer.remove(0);
            loginContainer.add("true");
            if (typeAction.equals(Type.LOGIN))
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
        while (!reader.ready()) {
        }
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

    private void sendPost(String post, OutputStream output, BufferedReader reader) throws IOException, InterruptedException {
        String msg = post.split(";")[4];
        if (!msg.isEmpty()) {
            String channelID = currentChannelID;
            int length = msg.length() + channelID.length() + 1;
            String request = length + ";2;" + channelID + ";" + msg;
            output.write(request.getBytes());
            int counter = 0;
            while (!reader.ready() && counter < 20) {
                Thread.currentThread().sleep(100);
                counter++;
            }
            if (!reader.ready()) {
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
        while (!reader.ready() && counter < 20) {
            Thread.currentThread().sleep(100);
            counter++;
        }
        synchronized (posts) {
            while (reader.ready()) {
                reader.read(buffor, 0, 2);
                int length = 0;
                if (buffor[1] == ';') {
                    length = Integer.valueOf(String.valueOf(buffor[0]));
                    reader.read(buffor, 0, 2);
                } else {
                    length = Integer.valueOf(String.valueOf(buffor[0])) * 10 + Integer.valueOf(String.valueOf(buffor[1]));
                    reader.read(buffor, 0, 1);
                    if (buffor[0] == ';') {
                        reader.read(buffor, 0, 2);
                    } else {
                        length *= 10;
                        length += Integer.valueOf(String.valueOf(buffor[0]));
                        reader.read(buffor, 0, 3);
                    }
                }
                reader.read(buffor, 0, length);
                String response = String.valueOf(buffor, 0, length);
                while (response.length() != length) {
                    reader.read(buffor, 0, length - response.length());
                    response += String.valueOf(buffor, 0, length);
                }
                posts.add(response.split(";")[1] + " said:");
                posts.add(response.split(";")[2]);
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
            System.out.println("All messages read");
        }
    }

    private void unsubscribeChannel(String request, OutputStream output, BufferedReader reader) throws IOException, InterruptedException {
        output.write(request.getBytes());
        int counter = 0;
        while (!reader.ready() && counter < 20) {
            Thread.currentThread().sleep(100);
            counter++;
        }
        if (!reader.ready()) {
            //TODO resend
        } else {
            reader.read(buffor, 0, 5);
        }
    }

    private void addChannel(String request, OutputStream output, BufferedReader reader) throws IOException, InterruptedException {
        output.write(request.getBytes());
        int counter = 0;
        while (!reader.ready() && counter < 20) {
            Thread.currentThread().sleep(100);
            counter++;
        }
        reader.read(buffor, 0, 5);
        if (buffor[4] == '0') {
            while (!reader.ready()) {
            }
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
                synchronized (channels) {
                    channels.add(new Channel(response));
                }
            }
        }
    }

    private void subscribeChannel(String data, OutputStream output, BufferedReader reader) throws IOException, InterruptedException {
        String[] splitted = data.split(";");
        String channelID = splitted[2];
        String channelName = splitted[3];
        String request = splitted[0] + ";4;" + splitted[2];
        output.write(request.getBytes());
        int counter = 0;
        while (!reader.ready() && counter < 20) {
            Thread.currentThread().sleep(100);
            counter++;
        }
//		reader.read(buffor, 0, 5);
        //TODO: temporary
        reader.read(buffor);
//		if(buffor[4] == '0') {
        channels.add(new Channel(channelID, channelName));
//		}
    }

    private void requestForAvailableChannels(String request, OutputStream output, BufferedReader reader) throws IOException, InterruptedException {
        output.write(request.getBytes());
        int counter = 0;
        List<String> tmp = new ArrayList<>();
        while (!reader.ready() && counter < 20) {
            Thread.currentThread().sleep(100);
            counter++;
        }
        while (!reader.ready()) {
        }
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
