import java.util.LinkedList;
import java.util.Queue;

class User {
    private int id;
    private boolean loginStatus;
    private Queue<String> messages;

    User(int id, boolean loginStatus) {
        this.id = id;
        this.loginStatus = loginStatus;
        messages = new LinkedList<>();
    }

    int getId() {
        return id;
    }

    void setId(int id) {
        this.id = id;
    }

    boolean isLoggedIn() {
        return loginStatus;
    }

    void setLoginStatus(boolean loginStatus) {
        this.loginStatus = loginStatus;
    }

    public Queue<String> getMessages() {
        return messages;
    }

    public void setMessages(Queue<String> messages) {
        this.messages = messages;
    }
}
