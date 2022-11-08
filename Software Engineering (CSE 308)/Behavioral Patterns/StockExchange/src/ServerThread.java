import java.io.*;
import java.net.*;
import java.util.*;

public class ServerThread implements Runnable {
    private BufferedReader br;
    private PrintWriter pw;
    private User user;
    private static ArrayList<Stock> stocks;
    private static ArrayList<User> users;

    private ServerThread(Socket socket) {
        try {
            br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            pw = new PrintWriter(socket.getOutputStream(), true);
        } catch (IOException e) {
            e.printStackTrace();
        }
        user = null;
    }

    public static void main(String[] args) {
        stocks = new ArrayList<>();
        users = new ArrayList<>();
        try {
            BufferedReader br = new BufferedReader(new FileReader("input.txt"));
            String line;
            while ((line = br.readLine()) != null) {
                String[] tokens = line.split(" ");
                stocks.add(new Stock(tokens[0], Integer.parseInt(tokens[1]), Float.parseFloat(tokens[2])));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        for (int i = 0; i < 3; i++) {
            users.add(new User());
        }

        new Thread(ServerThread::updateStock,"update stock thread").start();
        try {
            ServerSocket ss = new ServerSocket(1000);
            while (true) {
                Socket s = ss.accept();
                System.out.println("connected.");
                new Thread(new ServerThread(s)).start();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private static void updateStock() {
        Scanner sc = new Scanner(System.in);
        String[] tokens;
        Stock stock;
        while (true) {
            tokens = sc.nextLine().split(" ");
            switch (tokens[0]) {
                case "I":
                    stock = searchStock(tokens[1]);
                    if (stock != null) {
                        stock.updateValue(Float.parseFloat(tokens[2]));
                    }
                    break;
                case "D":
                    stock = searchStock(tokens[1]);
                    if (stock != null) {
                        stock.updateValue(Float.parseFloat(tokens[2]) * -1);
                    }
                    break;
                case "C":
                    stock = searchStock(tokens[1]);
                    if (stock != null) {
                        stock.updateCount(Integer.parseInt(tokens[2]));
                    }
                    break;
            }
        }
    }

    @Override
    public void run() {
        while (true) {
            try {
                String data = br.readLine();
                parseData(data);
            } catch (IOException e) {
                break;
            }
        }
    }

    private void parseData(String data) {
        String[] tokens = data.split(" ");
        Stock stock;
        switch (tokens[0]) {
            case "L":
                user = users.get(Integer.parseInt(tokens[1]));
                user.setServerThread(this);
                printStocks();
                break;
            case "S":
                if (user != null) {
                    stock = searchStock(tokens[1]);
                    if (stock != null) {
                        stock.subscribe(user);
                    }
                }
                break;
            case "U":
                if (user != null) {
                    stock = searchStock(tokens[1]);
                    if (stock != null) {
                        stock.unsubscribe(user);
                    }
                }
                break;
        }
    }

    private static Stock searchStock(String name) {
        for (Stock stock : stocks) {
            if (stock.getName().equals(name))
                return stock;
        }
        return null;
    }

    private void printStocks() {
        pw.println("Stock List:");
        for (Stock stock : stocks) {
            pw.println(stock.getName() + " " + stock.getCount() + " " + stock.getValue());
        }
    }

    PrintWriter getPrintWriter() {
        return pw;
    }
}