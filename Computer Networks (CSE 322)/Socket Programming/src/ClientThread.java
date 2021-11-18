import java.io.*;
import java.net.Socket;

class ClientThread extends Thread {
    private Socket socket;
    //private BufferedReader in;
    private ObjectInputStream in;

    ClientThread(Socket socket) {
        this.socket = socket;
        try {
//            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            in = new ObjectInputStream(socket.getInputStream());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void run() {
        while (true) {
            try {
                Object obj = in.readObject();
                if (obj.getClass().getSimpleName().equals("String")) {
                    parseMessage((String) obj);
                }
            } catch (IOException | ClassNotFoundException e) {
                e.printStackTrace();
            }
        }
    }

    private void parseMessage(String message) {
        String[] data = message.split(" ");
        //if (data.length == 0) return;
        switch (data[0]) {
            case "d":
                receiveFile(data[1], Integer.parseInt(data[2]), Integer.parseInt(data[3]));
                break;
            default:
                System.out.println(message);
        }
    }

    private void receiveFile(String filename, int fileSize, int maxChunkSize) {
        System.out.println(fileSize + " " + maxChunkSize);
        try {
            FileOutputStream fos = new FileOutputStream(filename);
            BufferedOutputStream bos = new BufferedOutputStream(fos);
            InputStream is = socket.getInputStream();

            byte[] contents = new byte[maxChunkSize];
            int bytesRead = 0;
            while (bytesRead < fileSize) {
                //bytesRead = is.read(contents);
                int chunkSize = is.read(contents);
                System.out.println(chunkSize);
                bos.write(contents, 0, chunkSize);
                bytesRead += chunkSize;
            }
            bos.flush();
        } catch (IOException e) {
            System.err.println("failed to download!");
        }
    }
}