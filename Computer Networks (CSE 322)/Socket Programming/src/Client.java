import java.io.*;
import java.net.Socket;
import java.util.Scanner;

public class Client {
    private static BufferedReader in;
    private static PrintWriter out;

    public static void main(String[] args) {
        Socket socket;
        try {
            socket = new Socket("localhost", 1000);
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            out = new PrintWriter(socket.getOutputStream(), true);
        } catch (IOException e) {
            e.printStackTrace();
        }

        new Thread(Client::receive, "receive thread").start();

        Scanner sc = new Scanner(System.in);
        while (true) {
            out.println(sc.nextLine());
        }
    }

    private static void receive() {
        while (true) {
            try {
                System.out.println(in.readLine());
            } catch (IOException e) {
                break;
            }
        }
    }
}
