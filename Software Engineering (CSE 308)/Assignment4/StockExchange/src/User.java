class User {
    private boolean loginStatus;

    User() {
        loginStatus = false;
    }

    void update(Stock stock, String property) {
        if (loginStatus)
            System.out.println(stock.getName() + " " + stock.getCount() + " " + stock.getValue()
                    + " (change in " + property + ")");
    }

    void toggleLoginStatus() {
        loginStatus = !loginStatus;
    }
}
