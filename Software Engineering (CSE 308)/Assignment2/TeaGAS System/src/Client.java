import builder.SystemDirector;
import system.System_;

import java.util.InputMismatchException;
import java.util.Scanner;

public class Client {
    private static String[] packages = {"package", "Silver", "Gold", "Diamond", "Platinum"};
    private static String[] internetConnections = {"internet connection", "Wifi", "Ethernet", "SimCard"};
    private static String[] frameworks = {"framework", "Django", "Spring", "Laravel"};

    public static void main(String[] args) {
        SystemDirector systemDirector = new SystemDirector();
        String packageName;
        String internetConnectionName;
        String frameworkName;
        Scanner sc = new Scanner(System.in);
        String command;
        while (true) {
            System.out.println("Press 0 to exit, any other key to continue:");
            command = sc.next();
            if (command.equals("0"))
                break;

            packageName = chooseItem(packages);
            internetConnectionName = chooseItem(internetConnections);
            frameworkName = chooseItem(frameworks);

            System_ system = systemDirector.construct(packageName, internetConnectionName, frameworkName);
        }
    }

    private static String chooseItem(String[] items) {
        int chosenItemNo;
        Scanner sc = new Scanner(System.in);
        System.out.println("Choose " + items[0]);
        for (int i = 1; i < items.length; i++) {
            System.out.println(i + ". " + items[i]);
        }
        while (true) {
            try {
                chosenItemNo = sc.nextInt();
            } catch (InputMismatchException e) {
                System.out.println("invalid input!");
                sc.nextLine();
                continue;
            }

            if (chosenItemNo < items.length) {
                return items[chosenItemNo];
            } else {
                System.out.println("invalid input!");
            }
        }
    }
}
