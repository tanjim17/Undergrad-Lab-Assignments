import java.io.*;
import java.net.Socket;

class ClientThread extends Thread {
    private Socket socket;
    //private BufferedReader in;
    private ObjectInputStream in;
    volatile static boolean ack;

    ClientThread(Socket socket) {
        this.socket = socket;
        try {
//            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            in = new ObjectInputStream(socket.getInputStream());
        } catch (IOException e) {
            e.printStackTrace();
        }
        ack = false;
    }

    public void run() {
        while (true) {
            try {
                Object obj = in.readObject();
                if (obj.getClass().getSimpleName().equals("String")) {
                    parseMessage((String) obj);
                }
            } catch (IOException | ClassNotFoundException e) {
                System.out.println("connection lost!");
                break;
            }
        }
    }

    private void parseMessage(String message) {
        String[] data = message.split(" ");
        switch (data[0]) {
            case "d":
                download(data[1], Integer.parseInt(data[2]), Integer.parseInt(data[3]));
                break;
            case "u":
                new UploadThread(socket, data[1], Integer.parseInt(data[2])).start();
                break;
            case "ok":
                ack = true;
                break;
            default:
                System.out.println(message);
        }
    }

    private void download(String filename, int fileSize, int maxChunkSize) {
        System.out.println(fileSize + " " + maxChunkSize);
        try {
            FileOutputStream fos = new FileOutputStream(filename);
            BufferedOutputStream bos = new BufferedOutputStream(fos);
            InputStream is = socket.getInputStream();

            byte[] contents = new byte[maxChunkSize];
            int bytesRead = 0;
            while (bytesRead < fileSize) {
                int chunkSize = is.read(contents);
                System.out.println(chunkSize);
                bos.write(contents, 0, chunkSize);
                bytesRead += chunkSize;
            }
            bos.flush();
            fos.close();
        } catch (IOException e) {
            System.err.println("failed to download!");
        }
    }
}