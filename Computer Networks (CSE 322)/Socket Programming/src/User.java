import java.util.LinkedList;

class User {
    private int id;
    private LinkedList<String> messages;
    private boolean loginStatus;

    User(int id, boolean loginStatus) {
        this.id = id;
        messages = new LinkedList<>();
        this.loginStatus = loginStatus;
    }

    int getId() {
        return id;
    }

    boolean isLoggedIn() {
        return loginStatus;
    }

    String showUnreadMessages() {
        StringBuilder allMessages = new StringBuilder();
        while (!messages.isEmpty()) {
            allMessages.append(messages.remove()).append("\n\n");
        }
        return allMessages.toString();
    }

    void receiveMessage(String message) {
        messages.add(message);
    }

    void setLoginStatus(boolean loginStatus) {
        this.loginStatus = loginStatus;
    }
}
