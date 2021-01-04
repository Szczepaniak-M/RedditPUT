package application;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;

public class Channel {
	private String id;
	private String name;
	private int numberOfNewMsgs = 0;
	private ObservableList<Integer> obsList = FXCollections.observableArrayList();
	
	public Channel(String response) {
		this.id = response.split(";")[0];
		this.name = response.split(";")[1];
		obsList.add(numberOfNewMsgs);
	}
	
	public String getId() {
		return id;
	}
	
	public String getName() {
		return name;
	}
	
	public void increaseNumberOfMsgs() {
		numberOfNewMsgs++;
		obsList.remove(0);
		obsList.add(numberOfNewMsgs);
	}
	
	public void clearNumberOfNewMsgs() {
		numberOfNewMsgs = 0;
		obsList.remove(0);
		obsList.add(numberOfNewMsgs);
	}
	
	public int getNumberOfNewMsgs() {
		return numberOfNewMsgs;
	}
	
	public ObservableList<Integer> getObsList() {
		return obsList;
	}
}