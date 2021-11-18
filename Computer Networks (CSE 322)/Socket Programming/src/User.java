import java.util.LinkedList;
import java.util.Queue;

class User {
    private int id;
    private Queue<String> messages;
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
        StringBuilder allMessages = new StringBuilder("unread messages:\n");
        while(!messages.isEmpty()) {
            allMessages.append(messages.remove()).append("\n");
        }
        return allMessages.toString();
    }

    void addUnreadMessage(String message) {
        messages.add(message);
    }

    ServerThread getServerThread() {
        return serverThread;
    }

    void setServerThread(ServerThread serverThread) {
        this.serverThread = serverThread;
    }
}
