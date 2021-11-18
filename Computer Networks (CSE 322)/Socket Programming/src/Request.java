public class Request {
    private int reqId;
    private int senderId;
    private String description;
    private static int lastReqID = 0;

    public Request(int senderId, String description) {
        reqId = ++lastReqID;
        this.senderId = senderId;
        this.description = description;
    }

    public int getReqId() {
        return reqId;
    }

    public int getSenderId() {
        return senderId;
    }

    public String getDescription() {
        return description;
    }

    public String toString() {
        return "new request: " + description + "\nsender: " + senderId;
    }
}
