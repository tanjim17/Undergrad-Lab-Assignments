import builder.SystemBuilder;
import builder.SystemDirector;
import system.System_;

import java.util.Scanner;

public class Client {
    public static void main(String [] args) {
        SystemDirector systemDirector = new SystemDirector();
        SystemBuilder systemBuilder;
        String packageName;
        String internetConnectionName;
        String frameworkName;
        Scanner sc = new Scanner(System.in);
        while (true) {
            System.out.println("Choose package: SilverBuilder, Gold, Diamond, Platinum\n" +
                    "Choose internet connection: Wifi, Ethernet, SimCard\n" +
                    "Choose framework: Django, Spring, Laravel");
            String[] input = sc.nextLine().split(" ");
            if(input[0].equalsIgnoreCase("exit")) {
                break;
            }
            if(input.length != 3) {
                continue;
            } else {
                packageName = input[0];
                internetConnectionName = input[1];
                frameworkName = input[2];
            }

            System_ system = systemDirector.construct(packageName, internetConnectionName, frameworkName);
        }
    }
}
