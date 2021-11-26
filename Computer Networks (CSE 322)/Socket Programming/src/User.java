import java.util.LinkedList;

class User {
    private int id;
    private LinkedList<String> messages;
    private ServerThread serverThread;

    User(int id, ServerThread serverThread) {
        this.id = id;
        messages = new LinkedList<>();
        this.serverThread = serverThread;
    }

    int getId() {
        return id;
    }

    boolean isLoggedIn() {
        return serverThread != null;
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

    void setServerThread(ServerThread serverThread) {
        this.serverThread = serverThread;
    }
}
