package application;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.Socket;

import javafx.scene.control.Label;

public class MyThread extends Thread {
    private Label label;

    public MyThread(Label label) {
        this.label = label;
    }

    @Override
    public void run() {
        try (Socket clientSocket = new Socket("127.0.0.1", 1234)) {
            BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            while (true) {
                label.setText(reader.readLine());
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
