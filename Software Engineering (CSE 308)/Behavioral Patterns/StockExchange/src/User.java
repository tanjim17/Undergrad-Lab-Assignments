class User {
    private ServerThread serverThread;

    void update(Stock stock, String property) {
            serverThread.getPrintWriter().println(stock.getName() + " " + stock.getCount() + " " + stock.getValue()
                    + " (change in " + property + ")");
    }

    void setServerThread(ServerThread serverThread) {
        this.serverThread = serverThread;
    }
}
