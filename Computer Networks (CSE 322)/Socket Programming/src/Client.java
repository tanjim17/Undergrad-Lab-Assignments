import java.io.*;
import java.net.Socket;
import java.util.Scanner;

public class Client {
    public static void main(String[] args) {
        Socket socket;
        //PrintWriter out;
        ObjectOutputStream out;
        try {
            socket = new Socket("localhost", 1000);
            //out = new PrintWriter(socket.getOutputStream(), true);
            out = new ObjectOutputStream(socket.getOutputStream());
            new ClientThread(socket).start();

            System.out.println("welcome!");
            Scanner sc = new Scanner(System.in);
            while (true) {
                //out.println(sc.nextLine());
                out.writeObject(sc.nextLine());
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
