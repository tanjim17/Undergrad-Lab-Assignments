public class Request {
    private int reqId;
    private int senderId;
    private String description;
    private static int lastReqId = 0;

    public Request(int senderId, String description) {
        reqId = ++lastReqId;
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
        return "request id: " + reqId + "\nrequest: " + description + "\nsender id: " + senderId;
    }
}
