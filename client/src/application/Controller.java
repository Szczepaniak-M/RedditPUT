package application;

import java.net.URL;
import java.util.ResourceBundle;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.TextField;

public class Controller implements Initializable {
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
    public void loginBtnOnClickListener(ActionEvent event) {
    	System.out.println("User " + tfLogin.getText() + " password " + tfPassword.getText()
    			+ " server address " + tfServerAddress.getText() + " port " + tfPort.getText());
    }
}
