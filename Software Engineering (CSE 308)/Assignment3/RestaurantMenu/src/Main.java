import pizza.*;
import pizza.decorator.*;

import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        Pizza pizza;
        String choice;
        while (true) {
            System.out.println("\n---------Menu---------\n1. Beef Pizza with French fry\n" +
                    "2. Veggi Pizza with onion rings\n" +
                    "3. A combo meal with Veggi Pizza, French Fry and Coke\n" +
                    "4. A combo meal with Veggi Pizza, Onion Rings and Coffee\n" +
                    "5. A Beef Pizza only\n" +
                    "Press serial no. to order, any other key to exit");
            choice = scanner.next();
            switch (choice) {
                case "1":
                    pizza = new BeefPizza();
                    pizza = new FrenchFries(pizza);
                    break;
                case "2":
                    pizza = new VeggiPizza();
                    pizza = new OnionRings(pizza);
                    break;
                case "3":
                    pizza = new VeggiPizza();
                    pizza = new FrenchFries(pizza);
                    pizza = new Coke(pizza);
                    break;
                case "4":
                    pizza = new VeggiPizza();
                    pizza = new OnionRings(pizza);
                    pizza = new Coffee(pizza);
                    break;
                case "5":
                    pizza = new BeefPizza();
                    break;
                default:
                    return;
            }
            System.out.println(pizza.getName() + ": " + pizza.getPrice() + " tk");
        }
    }
}
