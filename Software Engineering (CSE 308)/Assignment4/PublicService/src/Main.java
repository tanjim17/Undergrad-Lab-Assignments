import java.util.HashMap;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        JCC jcc = new JCC();

        JRTA jrta = new JRTA(jcc);
        JPDC jpdc = new JPDC(jcc);

        HashMap<String, Organization> organizationList = new HashMap<>();
        organizationList.put("JRTA", jrta);
        organizationList.put("JPDC", jpdc);

        Scanner sc = new Scanner(System.in);
        Organization currentOrg;
        while (true) {
            String command = sc.nextLine();
            String[] tokens = command.split(" ");
            currentOrg = organizationList.get(tokens[0]);

            if (tokens[1].equals("SERVE")) {
                currentOrg.provide();
            } else {
                currentOrg.request(tokens[1]);
            }

            if (command.equals("exit"))
                return;
        }
    }
}
