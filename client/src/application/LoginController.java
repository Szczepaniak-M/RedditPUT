package application;

import java.io.IOException;
import java.net.URL;
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
    public void loginBtnOnClickListener(ActionEvent event) throws IOException {
    	System.out.println("User " + tfLogin.getText() + " password " + tfPassword.getText()
    			+ " server address " + tfServerAddress.getText() + " port " + tfPort.getText());
    	//load main scene
    	Pane pane = FXMLLoader.load(getClass().getResource("MainWindow.fxml"));
    	rootPane.getChildren().setAll(pane);
    }
}
