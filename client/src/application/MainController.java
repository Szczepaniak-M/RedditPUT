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
import javafx.scene.control.TextField;
import javafx.scene.layout.Pane;

public class MainController implements Initializable {
	@FXML
	private Pane rootPane;
	
	@FXML
	private Button btnLogOut;
	
	@FXML
	private Label currentChannel;
	
	@FXML
	private TextField tfMessage;
	
	@FXML
	private ListView<String> listViewMessages;
	
	private List<String> communicationContainer;
	private CommunicationThread communicationThread;
	private ObservableList<String> posts = FXCollections.synchronizedObservableList(FXCollections.observableArrayList(new ArrayList<String>()));
	
	@Override
	public void initialize(URL arg0, ResourceBundle arg1) {
		generateChannelButtons(communicationContainer);
		communicationContainer.clear();		
	}
	
	public void initData(CommunicationThread communicationThread, List<String> communicationContainer) {
		this.communicationThread = communicationThread;
		this.communicationContainer = communicationContainer;
		communicationThread.setPosts(posts);		        
	}
	
	@FXML
	public void logoutBtnOnClickListener() throws IOException {
		communicationContainer.add(";logout;");
		System.out.println("Log Out");
		//load login scene
    	Pane pane = FXMLLoader.load(getClass().getResource("LoginWindow.fxml"));
    	rootPane.getChildren().setAll(pane);    	
	}
	
	public void sendBtnOnClickListener() {
		String content = tfMessage.getText();
		String channelName = currentChannel.getText();
		if(!channelName.isEmpty() && !content.isEmpty()) 
			synchronized (communicationContainer) {
				communicationContainer.add("x;2;x;" + channelName + ";" + content);
			}
		tfMessage.setText("");
	}
	
	private void generateChannelButtons(List<String> channels) {
		int startY = 180;
		int shiftY = 0;
		int positionX = 50;
		int height = 30;
		int width = 130;
		for(String s : channels) {
			Button b = new Button(s.split(";")[1]);
			b.setLayoutX(positionX);
			b.setLayoutY(startY + shiftY);
			b.setPrefHeight(height);
			b.setPrefWidth(width);
			b.setMnemonicParsing(false);
			b.setOnAction(e -> {
				currentChannel.setText(s.split(";")[1]);
				String channelId = s.split(";")[0];
				String requestForPosts = channelId.length() + ";8;" + channelId;
				posts.clear();
				synchronized (communicationContainer) {
					communicationContainer.add(requestForPosts);
				}					
				loadMessages();
			});
			rootPane.getChildren().add(b);
			shiftY += 40;
		}
	}
	
	private void loadMessages() {
		synchronized (posts) {
			int index = posts.size();
			listViewMessages.setItems(posts);
			listViewMessages.scrollTo(index);
		}
	}
}
