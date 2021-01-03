package application;

import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.ResourceBundle;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.ListView;
import javafx.scene.layout.Pane;

public class MainController implements Initializable {
	@FXML
	private Pane rootPane;
	
	@FXML
	private Button btnLogOut;
	
	@FXML
	private Label currentChannel;
	
	@FXML
	private ListView<String> listViewMessages;
	
	private List<String> communicationContainer;
	private CommunicationThread communicationThread;
	
	@Override
	public void initialize(URL arg0, ResourceBundle arg1) {
		generateChannelButtons(communicationContainer);
		
	}
	
	public void initData(CommunicationThread communicationThread, List<String> communicationContainer) {
		this.communicationThread = communicationThread;
		this.communicationContainer = communicationContainer;
	}
	
	@FXML
	public void logoutBtnOnClickListener() throws IOException {
		System.out.println("Log Out");
		
		//load login scene
    	Pane pane = FXMLLoader.load(getClass().getResource("LoginWindow.fxml"));
    	rootPane.getChildren().setAll(pane);    	
	}
	
	public void generateChannelButtons(List<String> channels) {
		int startY = 180;
		int shiftY = 0;
		int positionX = 50;
		int height = 30;
		int width = 130;
		for(String s : channels) {
			Button b = new Button(s);
			b.setLayoutX(positionX);
			b.setLayoutY(startY + shiftY);
			b.setPrefHeight(height);
			b.setPrefWidth(width);
			b.setMnemonicParsing(false);
			b.setOnAction(e -> {
				currentChannel.setText(s);
				loadMessages(s);
			});
			rootPane.getChildren().add(b);
			shiftY += 40;
		}
	}
	
	public void loadMessages(String channel) {
//		int startY = 240;
//		int shiftY = 0;
//		int positionX = 250;
//		int heightAuthor = 30;
//		int heightMessage = 60;
//		int width = 405;
		List<String> messages = new ArrayList<>();
		if(channel.equals("Halo halo")) {
			messages.add("Einstein said:");
			messages.add("OTW jest najprostsza");
			messages.add("Newton said:");
			messages.add("Nie, teoria grawitacji jest łatwiejsza");
			messages.add("Tesla said:");
			messages.add("Popieram newtona");
			messages.add("Einstein said:");
			messages.add("Ok.");
			messages.add("Tesla said:");
			messages.add("Dobrze, że się rozumiemy");
			messages.add("Galileo said:");
			messages.add("A co z moimi eksperymentami?");
			messages.add("Newton said:");
			messages.add("Nieważne, jabłko rozwiązało sprawę");
			messages.add("Einstein said:");
			messages.add("OTW jest najprostsza");
			messages.add("Newton said:");
			messages.add("Nie, teoria grawitacji jest łatwiejsza");
			messages.add("Tesla said:");
			messages.add("Popieram newtona");
			messages.add("Einstein said:");
			messages.add("Ok.");
			messages.add("Tesla said:");
			messages.add("Dobrze, że się rozumiemy");
			messages.add("Galileo said:");
			messages.add("A co z moimi eksperymentami?");
			messages.add("Newton said:");
			messages.add("Nieważne, jabłko rozwiązało sprawę");
		}
		ObservableList<String> items = FXCollections.observableArrayList(messages);
		int index = items.size();
		listViewMessages.setItems(items);
		listViewMessages.scrollTo(index);
	}
}
