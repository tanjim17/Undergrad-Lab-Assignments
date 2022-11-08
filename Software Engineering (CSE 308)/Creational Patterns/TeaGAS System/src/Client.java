import builder.SystemDirector;
import system.System_;

import java.util.*;

public class Client {
    private static final List<String> packages = new ArrayList<>(Arrays.asList("package", "Silver", "Gold", "Diamond", "Platinum"));
    private static List<String> internetConnections = new ArrayList<>(Arrays.asList("internet connection", "Wifi", "GSM"));
    private static final List<String>  frameworks = new ArrayList<>(Arrays.asList("framework", "Django", "Spring", "Laravel"));

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
            internetConnectionName = chooseItem(getInternetConnections(packageName));
            frameworkName = chooseItem(frameworks);

            System_ system = systemDirector.construct(packageName, internetConnectionName, frameworkName);

            system.measureWeight();
        }
    }

    private static String chooseItem(List<String> items) {
        int chosenItemNo;
        Scanner sc = new Scanner(System.in);
        System.out.println("Choose " + items.get(0));
        for (int i = 1; i < items.size(); i++) {
            System.out.println(i + ". " + items.get(i));
        }
        while (true) {
            try {
                chosenItemNo = sc.nextInt();
            } catch (InputMismatchException e) {
                System.out.println("invalid input!");
                sc.nextLine();
                continue;
            }

            if (chosenItemNo < items.size()) {
                return items.get(chosenItemNo);
            } else {
                System.out.println("invalid input!");
            }
        }
    }

    private static List<String> getInternetConnections(String packageName) {
        List<String> newInternetConnections = new ArrayList<>(internetConnections);
        switch (packageName) {
            case "Diamond":
            case "Platinum":
                newInternetConnections.add("Ethernet");
                break;
        }
        return newInternetConnections;
    }
}
