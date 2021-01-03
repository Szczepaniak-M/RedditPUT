package application;

import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.ResourceBundle;

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
    private Button btnSignUp;
	@FXML
    private TextField tfLogin;
	@FXML
    private TextField tfPassword;
	@FXML
    private TextField tfServerAddress;
	@FXML
    private TextField tfPort;
    
	private List<String> communicationContainer = new ArrayList<>();
	private Type type;
	
	private CommunicationThread communicationThread;
	
	@Override
	public void initialize(URL arg0, ResourceBundle arg1) {}
	
	@FXML
    public void loginBtnOnClickListener() throws IOException {
    	System.out.println("Log In User " + tfLogin.getText() + " server address " + tfServerAddress.getText() + " port " + tfPort.getText());
    	type = Type.LOGIN;
    	communicationContainer.add("false");
    	Thread currentThread = Thread.currentThread();
    	communicationThread = new CommunicationThread(
    			tfLogin.getText(),
    			tfPassword.getText(),
    			tfServerAddress.getText(),
    			Integer.valueOf(tfPort.getText()),
    			currentThread,
    			communicationContainer,
    			type);
    	    	
    	Thread t = new Thread(communicationThread);
    	t.setName("Communication Thread");
    	t.start();
    	try {
    		synchronized (currentThread) {
    			currentThread.wait(1000000);
    			if(communicationContainer.get(0).equals("true")) {
    				redirectToMainScene();
    			} else {
    				System.out.println("Incorrect password");
    			}
			}			
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
    }
	
	@FXML
	public void signUpBtnOnClickListner() throws IOException {
		System.out.println("Sign Up User " + tfLogin.getText() + " server address " + tfServerAddress.getText() + " port " + tfPort.getText());
    	type = Type.REGISTRATION;
    	communicationContainer.add("false");
    	Thread currentThread = Thread.currentThread();
    	communicationThread = new CommunicationThread(
    			tfLogin.getText(),
    			tfPassword.getText(),
    			tfServerAddress.getText(),
    			Integer.valueOf(tfPort.getText()),
    			currentThread,
    			communicationContainer,
    			type);
    	    	
    	Thread t = new Thread(communicationThread);
    	t.setName("Communication Thread");
    	t.start();
    	try {
    		synchronized (currentThread) {
    			currentThread.wait(100000);
    			if(communicationContainer.get(0).equals("true")) {
    				redirectToMainScene();
    			} else {
    				System.out.println("Account duplicated");    				
    			}
			}			
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
	
	public void redirectToMainScene() throws IOException {
		//pass references to another controller
    	MainController mainController = new MainController();
    	FXMLLoader loader = new FXMLLoader(getClass().getResource("MainWindow.fxml"));    	 
    	mainController.initData("Text from login controller", communicationThread, communicationContainer);
    	loader.setController(mainController);
    	
    	//load main scene
    	Pane pane = loader.load();
    	rootPane.getChildren().setAll(pane);
	}
}
