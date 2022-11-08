abstract class Organization {
    private JCC jcc;

    Organization(JCC jcc) {
        this.jcc = jcc;
    }

    void provide() {
        jcc.forwardService(this);
    }

    void request(String service) {
        jcc.forwardRequest(this, service);
    }

    void receive(String message) {
        System.out.println(message);
    }
}
