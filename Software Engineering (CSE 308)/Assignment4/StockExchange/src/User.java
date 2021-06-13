import java.io.PrintWriter;

class User {
    private PrintWriter pw;

    void update(Stock stock, String property) {
            pw.println(stock.getName() + " " + stock.getCount() + " " + stock.getValue()
                    + " (change in " + property + ")");
    }

    void setPrintWriter(PrintWriter pw) {
        this.pw = pw;
    }
}
