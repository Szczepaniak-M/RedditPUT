package application;

import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.ResourceBundle;

import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.control.Alert;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.control.Button;
import javafx.scene.control.TextField;
import javafx.scene.layout.Pane;
import javafx.stage.WindowEvent;

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

    private List<String> communicationContainer = Collections.synchronizedList(new ArrayList<>());
    private List<String> loginContainer = Collections.synchronizedList(new ArrayList<>());
    private Type type;
    private CommunicationThread communicationThread;    

    @Override
    public void initialize(URL arg0, ResourceBundle arg1) {
    }

    @FXML
    public void loginBtnOnClickListener() throws IOException {
        System.out.println("Log In User " + tfLogin.getText() + " server address " + tfServerAddress.getText() + " port " + tfPort.getText());
        type = Type.LOGIN;
        loginContainer.add("error");
        Thread currentThread = Thread.currentThread();
        communicationThread = new CommunicationThread(
                tfLogin.getText(),
                tfPassword.getText(),
                tfServerAddress.getText(),
                Integer.valueOf(tfPort.getText()),
                currentThread,
                loginContainer,
                type,
                communicationContainer);

        Thread t = new Thread(communicationThread);
        t.setName("Communication Thread");
        t.setDaemon(true);
        t.start();
        try {
            synchronized (currentThread) {
                currentThread.wait(5000);
                synchronized (loginContainer) {
                	if (loginContainer.get(0).equals("true")) {
                        loginContainer.clear();
                        redirectToMainScene();
                    } else if(loginContainer.get(0).equals("false")){
                    	loginContainer.clear();
                    	createErrorDialog("Invalid login or password");
                    } else {                        
                        loginContainer.clear();
                        createErrorDialog("Server didn't answer");
                    }
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
        loginContainer.add("error");
        Thread currentThread = Thread.currentThread();
        communicationThread = new CommunicationThread(
                tfLogin.getText(),
                tfPassword.getText(),
                tfServerAddress.getText(),
                Integer.valueOf(tfPort.getText()),
                currentThread,
                loginContainer,
                type,
                communicationContainer);

        Thread t = new Thread(communicationThread);
        t.setName("Communication Thread");
        t.setDaemon(true);
        t.start();
        try {
            synchronized (currentThread) {
                currentThread.wait(5000);
                synchronized (loginContainer) {
                	if (loginContainer.get(0).equals("true")) {
                        loginContainer.clear();
                        redirectToMainScene();
                    } else if(loginContainer.get(0).equals("false")){
                    	loginContainer.clear();
                    	createErrorDialog("Account duplicated");
                    } else {                        
                        loginContainer.clear();
                        createErrorDialog("Server didn't answer");
                    }
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
        mainController.initData(communicationThread, communicationContainer);
        loader.setController(mainController);

        //load main scene
        Pane pane = loader.load();
        rootPane.getChildren().setAll(pane);
        rootPane.getScene().getWindow().addEventHandler(javafx.stage.WindowEvent.WINDOW_CLOSE_REQUEST, this::closeWindow);
    }

    private void closeWindow(WindowEvent event) {
        System.out.println("Close application");
        communicationContainer.add(";logout;");
    }
    
    private void createErrorDialog(String content) {
		Alert alert = new Alert(AlertType.ERROR);
		alert.setTitle("Error dialog");
		alert.setHeaderText("Error");
		alert.setContentText(content);

		alert.showAndWait();
	}
}
