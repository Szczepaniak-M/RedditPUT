package application;

import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.ResourceBundle;

import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.collections.ListChangeListener;
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
	private Button btnLoadMsgs;
	@FXML
	private Button btnAddChannel;
	@FXML
	private Button btnRemoveChannel;
	@FXML
	private Label currentChannel;
	@FXML
	private TextField tfMessage;
	@FXML
	private ListView<String> listViewMessages;
	
	private List<String> communicationContainer;
	private CommunicationThread communicationThread;
	private ObservableList<String> posts = FXCollections.synchronizedObservableList(FXCollections.observableArrayList(new ArrayList<String>()));
	private ObservableList<String> numberOfNewMsgs = FXCollections.synchronizedObservableList(FXCollections.observableArrayList(new ArrayList<String>()));
	private List<Channel> channels;
	
	@Override
	public void initialize(URL arg0, ResourceBundle arg1) {
		channels = communicationThread.getChannels();
		generateChannelButtons(communicationContainer);
		communicationContainer.clear();				
		numberOfNewMsgs.addListener(new ListChangeListener<String>() {
			@Override
			public void onChanged(Change<? extends String> change) {				
				while(change.next()) {
					if(change.wasAdded()) {
						synchronized (numberOfNewMsgs) {
							Platform.runLater(() -> {
								btnLoadMsgs.setText("Load " + numberOfNewMsgs.get(0) + " new messages");
							});
						}						
					}									
				}
			}
		});
		numberOfNewMsgs.add("");
	}
	
	public void initData(CommunicationThread communicationThread, List<String> communicationContainer) {
		this.communicationThread = communicationThread;
		this.communicationContainer = communicationContainer;
		communicationThread.setObservables(posts, numberOfNewMsgs);		        
	}
	
	@FXML
	public void logoutBtnOnClickListener() throws IOException {
		communicationContainer.add(";logout;");
		System.out.println("Log Out");
		//load login scene
    	Pane pane = FXMLLoader.load(getClass().getResource("LoginWindow.fxml"));
    	rootPane.getChildren().setAll(pane);    	
	}
	
	@FXML
	public void sendBtnOnClickListener() {
		String content = tfMessage.getText();
		String channelName = currentChannel.getText();
		if(!channelName.isEmpty() && !content.isEmpty()) 
			synchronized (communicationContainer) {
				communicationContainer.add("x;2;x;" + channelName + ";" + content);
			}
		tfMessage.setText("");
	}
	
	@FXML
	public void btnLoadMsgsListener() {
		String requestForNewMsgs = ";n;";
		posts.clear();
		synchronized (communicationContainer) {
			communicationContainer.add(requestForNewMsgs);
		}					
		printMessages();
	}
	
	private void generateChannelButtons(List<String> channelNames) {
		int startY = 220;
		int shiftY = 0;
		int positionX = 50;
		int height = 30;
		int width = 130;
		for(String s : channelNames) {
			String channelID = s.split(";")[0];
			String channelName = s.split(";")[1];
			Button b = new Button();
			b.setLayoutX(positionX);
			b.setLayoutY(startY + shiftY);
			b.setPrefHeight(height);
			b.setPrefWidth(width);
			b.setMnemonicParsing(false);
			for(Channel c : channels) {
				if(c.getId().equals(channelID)) {
					b.setText(channelName + " | [" + c.getObsList().get(0) + "]");
					c.getObsList().addListener(new ListChangeListener<Integer>() {
						@Override
						public void onChanged(Change<? extends Integer> change) {
							while(change.next()) {
								if(change.wasAdded()) {
									Platform.runLater(() -> {
										b.setText(channelName + " | [" + c.getObsList().get(0) + "]");
									});										
								}
							}
						}
					});
				}
			}		
			b.setOnAction(e -> {
				currentChannel.setText(channelName);				
				String requestForPosts = channelID.length() + ";8;" + channelID;
				posts.clear();
				synchronized (communicationContainer) {
					communicationContainer.add(requestForPosts);
				}					
				printMessages();
			});
			rootPane.getChildren().add(b);
			shiftY += 40;
		}
	}
	
	private void printMessages() {
		synchronized (posts) {
			int index = posts.size();
			listViewMessages.setItems(posts);
			listViewMessages.scrollTo(index);
		}
	}
}
