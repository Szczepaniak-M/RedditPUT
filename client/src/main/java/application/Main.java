package application;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

import java.io.IOException;


public class Main extends Application {
    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage primaryStage) throws IOException {
        System.out.println("Welcome to RedditPUT");
        Parent root = FXMLLoader.load(getClass().getResource("../LoginWindow.fxml"));
        primaryStage.setTitle("RedditPUT");
        Scene scene = new Scene(root, 800, 680);
        scene.getStylesheets().add("style.css");
        primaryStage.setScene(scene);
        primaryStage.show();
    }
}
