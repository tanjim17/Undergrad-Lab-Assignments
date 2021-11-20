import java.io.*;
import java.net.Socket;
import java.util.Scanner;

public class Client {
    public static void main(String[] args) {
        Socket socket;
        //PrintWriter out;
        try {
            socket = new Socket("localhost", 1000);
            //out = new PrintWriter(socket.getOutputStream(), true);
            ObjectOutputStream out = new ObjectOutputStream(socket.getOutputStream());
            new ClientThread(socket).start();

            System.out.println("welcome!");
            Scanner sc = new Scanner(System.in);
            while (true) {
                String command = sc.nextLine();
                String[] data = command.split(" ");
                if(data[0].equals("u")) {
                    if(data.length == 3) command += " 0"; //req id = 0 i.e. not fulfilling any request
                    command += " " + new File(data[1]).length(); //appending file size during upload
                }
                out.writeObject(command);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
