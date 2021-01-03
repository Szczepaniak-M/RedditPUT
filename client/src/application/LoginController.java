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
    	
    	CommunicationThread communicationThread = new CommunicationThread(
    			tfLogin.getText(),
    			tfPassword.getText(),
    			tfServerAddress.getText(),
    			Integer.valueOf(tfPort.getText()));
    	
    	Thread ct = Thread.currentThread();
    	Thread t = new Thread(communicationThread);
    	t.setName("Communication Thread");
    	t.start();
    }
		
	public void redirectToMainScene() throws IOException {
		//pass references to another controller
    	MainController mainController = new MainController();
    	FXMLLoader loader = new FXMLLoader(getClass().getResource("MainWindow.fxml"));    	 
    	mainController.initData("Text from login controller");
    	loader.setController(mainController);
    	//load main scene
    	Pane pane = loader.load();
    	rootPane.getChildren().setAll(pane);
	}
}
