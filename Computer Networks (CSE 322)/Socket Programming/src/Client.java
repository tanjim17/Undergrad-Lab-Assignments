import java.io.*;
import java.net.Socket;
import java.util.Scanner;

public class Client {
    public static void main(String[] args) {
        try {
            Socket socket = new Socket("localhost", 1000);
            socket.setSoTimeout(5000);
            Socket fileSocket = new Socket("localhost", 1000);
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            new ClientThread(socket, fileSocket, out).start();

            System.out.println("welcome!");
            Scanner sc = new Scanner(System.in);
            while (true) {
                String command = sc.nextLine();
                String[] data = command.split(" ");
                if (data[0].equals("u")) {
                    if (data.length == 3) command += " 0"; //req id = 0 i.e. not fulfilling any request
                    command += " " + new File(data[1]).length(); //appending file size during upload
                }
                out.println(command);
            }
        } catch (IOException e) {
            System.out.println("could't connect to server!");
        }
    }
}
