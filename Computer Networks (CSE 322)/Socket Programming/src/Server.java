import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

class Server {
    public static void main(String[] args) throws IOException {
        ServerSocket serverSocket = new ServerSocket(1000);
        while (true) {
            System.out.println("waiting for connection...");
            Socket socket = serverSocket.accept();
            Socket fileSocket = serverSocket.accept();
            System.out.println("connection established!");
            new ServerThread(socket, fileSocket).start();
        }
    }
}
