public class Request {
    private int reqId;
    private int senderId;
    private String description;
    private static int lastReqId = 0;

    Request(int senderId, String description) {
        reqId = ++lastReqId;
        this.senderId = senderId;
        this.description = description;
    }

    int getReqId() {
        return reqId;
    }

    int getSenderId() {
        return senderId;
    }

    public String toString() {
        return "request id: " + reqId + "\nrequest: " + description + "\nsender id: " + senderId;
    }
}
