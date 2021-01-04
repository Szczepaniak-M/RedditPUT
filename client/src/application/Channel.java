package application;

public class Channel {
	private String id;
	private String name;
	
	public Channel(String response) {
		this.id = response.split(";")[0];
		this.name = response.split(";")[1];
	}
	
	public String getId() {
		return id;
	}
	public String getName() {
		return name;
	}	
}