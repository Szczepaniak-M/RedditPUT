package application;

import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.ResourceBundle;

import javafx.application.Platform;
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
        createCommunicationThread("Invalid login or password");
    }

    @FXML
    public void signUpBtnOnClickListener() throws IOException {
        System.out.println("Sign Up User " + tfLogin.getText() + " server address " + tfServerAddress.getText() + " port " + tfPort.getText());
        type = Type.REGISTRATION;
        createCommunicationThread("Account duplicated");
    }

    private void createCommunicationThread(String s) throws IOException {
        if (!tfPort.getText().matches("[0-9]+")) {
            createErrorDialog("Port must contain only numbers");
            return;
        } else if (!tfLogin.getText().matches("[a-zA-Z0-9!@#$%&*()_+=|<>?{}\\\\[\\\\]~-]+")) {
            createErrorDialog("Login must contain only latin characters, numbers or special characters");
            return;
        } else if (!tfPassword.getText().matches("[a-zA-Z0-9!@#$%&*()_+=|<>?{}\\\\[\\\\]~-]+")) {
            createErrorDialog("Password must contain only latin characters, numbers or special characters");
            return;
        }else if (!tfServerAddress.getText().matches("[a-zA-Z0-9!@#$%&*.()_+=|<>?{}\\\\[\\\\]~-]+")) {
            createErrorDialog("Server Address must contain only latin characters, numbers or special characters");
            return;
        }
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
                        } else if (loginContainer.get(0).equals("false")) {
                            loginContainer.clear();
                            createErrorDialog(s);
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

    private void redirectToMainScene() throws IOException {
        //pass references to another controller
        MainController mainController = new MainController();
        FXMLLoader loader = new FXMLLoader(getClass().getClassLoader().getResource("MainWindow.fxml"));
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
        // KDE error: not showing dialog - fix
        alert.setResizable(true);
        alert.onShownProperty().addListener(e -> {
            Platform.runLater(() -> alert.setResizable(false));
        });
        // End of fix
		alert.showAndWait();
	}
}
