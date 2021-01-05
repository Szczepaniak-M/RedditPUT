package application;

import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.collections.ListChangeListener;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.control.*;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.layout.Pane;

import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.ResourceBundle;

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
    private Button btnSubscribeChannel;
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
    private ObservableList<Channel> channels;
    private List<Button> buttons = new ArrayList<>();
    private List<Label> labels = new ArrayList<>();
    private ObservableList<String> availableChannels;
    private ObservableList<String> errors = FXCollections.synchronizedObservableList(FXCollections.observableArrayList(new ArrayList<>()));
    private int postsIndex;

    @Override
    public void initialize(URL arg0, ResourceBundle arg1) {
        channels = communicationThread.getChannels();
        availableChannels = communicationThread.getAvailableChannels();
        generateChannelButtons(channels);
        communicationContainer.clear();
        numberOfNewMsgs.addListener(new ListChangeListener<String>() {
            @Override
            public void onChanged(Change<? extends String> change) {
                while (change.next()) {
                    if (change.wasAdded()) {
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
        channels.addListener((ListChangeListener<Channel>) change -> {
            while (change.next()) {
                if (change.wasAdded()) {
                    Channel c = change.getAddedSubList().get(0);
                    double positionY;
                    if (buttons.isEmpty())
                        positionY = 260;
                    else
                        positionY = buttons.get(buttons.size() - 1).getLayoutY() + 40;
                    int positionX = 50;
                    int height = 30;
                    int width = 130;
                    Button b = getButton(c, positionY, positionX, height, width);
                    Label l = getLabel(c, positionY, positionX, height, width);
                    c.getObsList().addListener((ListChangeListener<Integer>) change1 -> {
                        while (change1.next()) {
                            if (change1.wasAdded()) {
                                Platform.runLater(() -> l.setText(c.getObsList().get(0).toString()));
                            }
                        }
                    });
                    b.setOnAction(e -> {
                        currentChannel.setText(c.getName());
                        String requestForPosts = c.getId().length() + ";8;" + c.getId();
                        posts.clear();
                        synchronized (communicationContainer) {
                            communicationContainer.add(requestForPosts);
                        }
                        printMessages();
                    });
                    Platform.runLater(() -> {
                        rootPane.getChildren().add(b);
                        rootPane.getChildren().add(l);
                    });
                    buttons.add(b);
                    labels.add(l);
                }
            }
        });
        availableChannels.addListener(new ListChangeListener<String>() {
            @Override
            public void onChanged(Change<? extends String> change) {
                while (change.next()) {
                    if (change.wasAdded()) {
                        List<String> names = new ArrayList<>();
                        List<String> ids = new ArrayList<>();
                        availableChannels.forEach(c -> {
                            ids.add(c.split(";")[0]);
                            names.add(c.split(";")[1]);
                        });
                        if (ids.size() == 1 && ids.get(0).equals("0")) {
                            Platform.runLater(() -> {
                                createAlertDialog(AlertType.INFORMATION, "Information Dialolg", "Info", "There is no possible channel to subscribe");
                            });
                            return;
                        }
                        Platform.runLater(() -> {
                            Dialog dialog = getChoiceDialog(names, "Subscribe Channel", "Please choose channel to subscribe");
                            Optional<String> result = dialog.showAndWait();
                            if (result.isPresent()) {
                                int index = names.indexOf(result.get());
                                String channelID = availableChannels.get(index).split(";")[0];
                                String request = channelID.length() + ";4;" + channelID + ";" + result.get();
                                synchronized (communicationContainer) {
                                    communicationContainer.add(request);
                                }
                            }
                        });
                    }
                }
            }
        });
        errors.addListener(new ListChangeListener<String>() {
            @Override
            public void onChanged(Change<? extends String> change) {
                synchronized (errors) {
                    while (change.next()) {
                        if (change.wasAdded()) {
                            String errorHeader = change.getAddedSubList().get(0);
                            String errorContent = change.getAddedSubList().get(1);
                            System.err.println(errorHeader + ": " + errorContent);
                            Platform.runLater(() -> {
                                createAlertDialog(AlertType.ERROR, "Error dialog", errorHeader, errorContent);
                            });
                            errors.clear();
                        }
                    }
                }
            }
        });
        posts.addListener(new ListChangeListener<String>() {
            @Override
            public void onChanged(Change<? extends String> change) {
                while (change.next()) {
                    if (change.wasAdded()) {
                        synchronized (posts) {
                            postsIndex = posts.size();
                            listViewMessages.scrollTo(postsIndex);
                        }
                    }
                }
            }
        });
    }

    public void initData(CommunicationThread communicationThread, List<String> communicationContainer) {
        this.communicationThread = communicationThread;
        this.communicationContainer = communicationContainer;
        communicationThread.setObservables(posts, numberOfNewMsgs, errors);
    }

    @FXML
    public void btnAddChannelClickListener() {
        Dialog dialog = getTextDialog("Add channel", "Enter channel name");
        Optional<String> result = dialog.showAndWait();
        if (result.isPresent() && result.get().matches("[a-zA-Z0-9!@#$%&*()_+=|<>?{}\\\\[\\\\]~-]+")) {
            if (!result.get().equals("")) {
                String request = result.get().length() + ";3;" + result.get();
                synchronized (communicationContainer) {
                    communicationContainer.add(request);
                }
            }
        } else {
            createAlertDialog(AlertType.ERROR, "Error dialog", "Wrong characters",
                    "Message must contain only latin characters, numbers or special characters");
        }
    }

    @FXML
    public void btnSubscribeChannelClickListener() {
        synchronized (communicationContainer) {
            communicationContainer.add("0;9;");
        }
    }

    @FXML
    public void btnRemoveChannelClickListener() {
        List<String> names = new ArrayList<>();
        for (Channel c : channels) {
            names.add(c.getName());
        }
        if (names.isEmpty()) {
            Platform.runLater(() -> {
                createAlertDialog(AlertType.INFORMATION, "Information Dialolg", "Info", "There is no possible channel to unsubscribe");
            });
            return;
        }
        Dialog dialog = getChoiceDialog(names, "Remove Channel", "Please choose channel to unsubscribe");

        Optional<String> result = dialog.showAndWait();
        if (result.isPresent()) {
            int index = -1;
            for (Button b : buttons) {
                if (b.getId().equals(result.get())) {
                    index = buttons.indexOf(b);
                    rootPane.getChildren().remove(b);
                    break;
                }
            }
            for (Label l : labels) {
                if (l.getId().equals(result.get() + "-label")) {
                    rootPane.getChildren().remove(l);
                    break;
                }
            }
            for (int i = 0; i < buttons.size(); i++) {
                if (i > index) {
                    Button btn = buttons.get(i);
                    btn.setLayoutY(btn.getLayoutY() - 40);
                    Label lbl = labels.get(i);
                    lbl.setLayoutY(lbl.getLayoutY() - 40);
                }
            }
            buttons.remove(index);
            labels.remove(index);
            for (Channel c : channels) {
                if (c.getName().equals(result.get())) {
                    String request = c.getId().length() + ";5;" + c.getId();
                    synchronized (communicationContainer) {
                        communicationContainer.add(request);
                    }
                    channels.remove(c);
                    break;
                }
            }
        }
    }

    @FXML
    public void logoutBtnOnClickListener() throws IOException {
        communicationContainer.add(";logout;");
        System.out.println("Log Out");
        //load login scene
        Pane pane = FXMLLoader.load(getClass().getResource("../../resources/LoginWindow.fxml"));
        rootPane.getChildren().setAll(pane);
    }

    @FXML
    public void sendBtnOnClickListener() {
        String content = tfMessage.getText();
        if (content.matches("[a-zA-Z0-9!@#$%&*()_+=|<>?{}\\\\[\\\\]~-]+")) {
            String channelName = currentChannel.getText();
            if (!channelName.isEmpty() && !content.isEmpty())
                synchronized (communicationContainer) {
                    communicationContainer.add("x;2;x;" + channelName + ";" + content);
                }
            tfMessage.setText("");
        } else {
            createAlertDialog(AlertType.ERROR, "Error dialog", "Wrong characters", "Message must contain only latin characters, numbers or special characters");
        }
    }

    @FXML
    public void btnLoadMsgsListener() {
        if (currentChannel.getText().equals("")) {
            System.out.println("Can't load messages from empty channel");
        } else {
            String requestForNewMsgs = ";n;";
            posts.clear();
            synchronized (communicationContainer) {
                communicationContainer.add(requestForNewMsgs);
            }
            printMessages();
        }
    }

    private void generateChannelButtons(List<Channel> channels) {
        int startY = 260;
        int shiftY = 0;
        int positionX = 50;
        int height = 30;
        int width = 130;
        for (Channel c : channels) {
            String channelID = c.getId();
            String channelName = c.getName();
            Button b = getButton(c, startY + shiftY, positionX, height, width);
            Label l = getLabel(c, startY + shiftY, positionX, height, width);
            c.getObsList().addListener((ListChangeListener<Integer>) change -> {
                while (change.next()) {
                    if (change.wasAdded()) {
                        Platform.runLater(() -> l.setText(c.getObsList().get(0).toString()));
                    }
                }
            });
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
            rootPane.getChildren().add(l);
            buttons.add(b);
            labels.add(l);
            shiftY += 40;
        }
    }

    private void printMessages() {
        synchronized (posts) {
            Platform.runLater(() -> {
                listViewMessages.setItems(posts);
            });
        }
    }

    private Button getButton(Channel c, double positionY, int positionX, int height, int width) {
        Button b = new Button();
        b.setId(c.getName());
        b.setLayoutX(positionX);
        b.setLayoutY(positionY);
        b.setPrefHeight(height);
        b.setPrefWidth(width);
        b.setMnemonicParsing(false);
        b.setText(c.getName());
        return b;
    }

    private Label getLabel(Channel c, double positionY, int positionX, int height, int width) {
        Label l = new Label();
        l.setId(c.getName() + "-label");
        l.setLayoutX(positionX + width + 10);
        l.setLayoutY(positionY);
        l.setPrefHeight(height);
        l.setPrefWidth(height);
        l.setMnemonicParsing(false);
        l.setText(c.getObsList().get(0).toString());
        return l;
    }

    private Dialog getTextDialog(String title, String headerText) {
        Dialog dialog = new TextInputDialog();
        dialog.setTitle(title);
        dialog.setHeaderText(headerText);
        // KDE error: not showing dialog - fix
        dialog.setResizable(true);
        dialog.onShownProperty().addListener(e -> {
            Platform.runLater(() -> dialog.setResizable(false));
        });
        // End of fix
        return dialog;
    }

    private Dialog getChoiceDialog(List<String> names, String s, String s2) {
        Dialog dialog = new ChoiceDialog(names.get(0), names);
        dialog.setTitle(s);
        dialog.setHeaderText(s2);
        // KDE error: not showing dialog - fix
        dialog.setResizable(true);
        dialog.onShownProperty().addListener(e -> {
            Platform.runLater(() -> dialog.setResizable(false));
        });
        // End of fix
        return dialog;
    }

    private void createAlertDialog(AlertType type, String title, String header, String content) {
        Alert alert = new Alert(type);
        alert.setTitle(title);
        alert.setHeaderText(header);
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
