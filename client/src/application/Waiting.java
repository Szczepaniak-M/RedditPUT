package application;

import java.time.Instant;

public class Waiting {
	private String type;
	private String request;
	private Instant creation;
	private String extras;
	
	public Waiting() {}
	
	public Waiting(String type, String request) {
		this.type = type;
		this.request = request;
		this.creation = Instant.now();
	}
	
	public Waiting(String type, String request, String extras) {
		this(type, request);
		this.extras = extras;
	}
	
	public String getType() {
		return type;
	}
	
	public String getRequest() {
		return request;
	}
	
	public Instant getCreation() {
		return creation;
	}
	
	public void setNewCreation(Instant creation) {
		this.creation = creation;
	}
	
	public String getExtras() {
		return extras;
	}
}
