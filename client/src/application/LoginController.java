package application;

import java.io.IOException;
import java.io.OutputStream;
import java.net.ConnectException;
import java.net.Socket;
import java.net.URL;
import java.net.UnknownHostException;
import java.util.ResourceBundle;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.TextField;
import javafx.scene.layout.Pane;

public class LoginController implements Initializable {
	@FXML
	private Pane rootPane;
	@FXML
    private Button btnLogIn;
	@FXML
    private TextField tfLogin;
	@FXML
    private TextField tfPassword;
	@FXML
    private TextField tfServerAddress;
	@FXML
    private TextField tfPort;
    
	@Override
	public void initialize(URL arg0, ResourceBundle arg1) {
		// TODO Auto-generated method stub	
	}
	
	@FXML
    public void loginBtnOnClickListener() throws IOException {
    	System.out.println("User " + tfLogin.getText() + " server address " + tfServerAddress.getText() + " port " + tfPort.getText());
    	//set connection
    	try (Socket clientSocket = new Socket(tfServerAddress.getText(), Integer.valueOf(tfPort.getText()))) {
    		OutputStream output = clientSocket.getOutputStream();
    		int loginLength = tfLogin.getText().length();
    		int passwordLength = tfPassword.getText().length();
    		int totalLength = loginLength + passwordLength + 1;
    		String request = totalLength + ";0;" + tfLogin.getText() + ";" + tfPassword.getText();
    		System.out.println(request);
    		byte[] loginInBytes = request.getBytes();
    		output.write(loginInBytes);
    	} catch (ConnectException ex) {
            System.out.println("Server not found: " + ex.getMessage());
    	}
    	
    	//load main scene
    	Pane pane = FXMLLoader.load(getClass().getResource("MainWindow.fxml"));
    	rootPane.getChildren().setAll(pane);
    }
}
