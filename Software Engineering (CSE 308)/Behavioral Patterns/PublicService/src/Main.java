import java.util.HashMap;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        JCC jcc = new JCC();

        JWSA jwsa = new JWSA(jcc);
        JPDC jpdc = new JPDC(jcc);
        JRTA jrta = new JRTA(jcc);
        JTRC jtrc = new JTRC(jcc);

        HashMap<String, Organization> organizationList = new HashMap<>();
        organizationList.put("JWSA", jwsa);
        organizationList.put("JPDC", jpdc);
        organizationList.put("JRTA", jrta);
        organizationList.put("JTRC", jtrc);

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
