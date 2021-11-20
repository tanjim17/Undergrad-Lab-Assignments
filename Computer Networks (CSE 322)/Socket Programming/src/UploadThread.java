import java.io.*;
import java.net.Socket;

class UploadThread extends Thread {
    private Socket socket;
    private String fileName;
    private int maxChunkSize;

    UploadThread(Socket socket, String fileName, int maxChunkSize) {
        this.socket = socket;
        this.fileName = fileName;
        this.maxChunkSize = maxChunkSize;
    }

    public void run() {
        try {
            File file = new File(fileName);
            FileInputStream fis = new FileInputStream(file);
            BufferedInputStream bis = new BufferedInputStream(fis);
            OutputStream os = socket.getOutputStream();
            long bytesSent = 0;
            byte[] chunk = new byte[maxChunkSize];
            ClientThread.ack = false;
            while (bytesSent < file.length()) {
                int chunkSize = bis.read(chunk);
                os.write(chunk, 0, chunkSize);
                System.out.println(chunkSize);
                bytesSent += chunkSize;
                int count = 0;
                while(count < 30) {
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    count++;
                    System.out.println("count" + count);
                    if(ClientThread.ack) {
                        ClientThread.ack = false;
                        break;
                    }
                }
                if(count == 30) {
                    os.write("timeout".getBytes());
                    break;
                }
            }
            os.write("completed".getBytes());
            fis.close();
            os.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
