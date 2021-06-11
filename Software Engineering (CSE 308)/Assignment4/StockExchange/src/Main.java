import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Scanner;

public class Main {
    private static ArrayList<Stock> stocks;

    public static void main(String[] args) {
        stocks = new ArrayList<>();
        ArrayList<User> users = new ArrayList<>();
        Scanner sc = new Scanner(System.in);

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

        User currentUser = users.get(0);
        currentUser.toggleLoginStatus();
        printStocks();
        String command;
        Stock stock;
        while (true) {
            command = sc.nextLine();
            String[] tokens = command.split(" ");
            switch (tokens[0]) {
                case "L": //login
                    currentUser.toggleLoginStatus();
                    currentUser = users.get(Integer.parseInt(tokens[1]));
                    currentUser.toggleLoginStatus();
                    printStocks();
                    break;
                case "S":
                    stock = searchStock(tokens[1]);
                    if (stock != null) {
                        stock.subscribe(currentUser);
                    }
                    break;
                case "U":
                    stock = searchStock(tokens[1]);
                    if (stock != null) {
                        stock.unsubscribe(currentUser);
                    }
                    break;
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
                default:
                    return;
            }
        }
    }

    private static Stock searchStock(String name) {
        for (Stock stock : stocks) {
            if (stock.getName().equals(name))
                return stock;
        }
        return null;
    }

    private static void printStocks() {
        System.out.println("Stock List:");
        for (Stock stock : stocks) {
            System.out.println(stock.getName() + " " + stock.getCount() + " " + stock.getValue());
        }
    }
}
