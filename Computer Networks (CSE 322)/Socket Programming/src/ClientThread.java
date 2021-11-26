import java.io.*;
import java.net.Socket;
import java.net.SocketTimeoutException;

class ClientThread extends Thread {
    private Socket fileSocket;
    private BufferedReader in;
    private PrintWriter out;
    private volatile boolean ack;
    private volatile boolean isTimeOut;
    private volatile boolean isUploading;

    ClientThread(Socket socket, Socket fileSocket, PrintWriter out) {
        this.fileSocket = fileSocket;
        try {
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        } catch (IOException e) {
            e.printStackTrace();
        }
        this.out = out;
        ack = isTimeOut = isUploading = false;
    }

    public void run() {
        while (true) {
            try {
                parseMessage(in.readLine());
            } catch (SocketTimeoutException e) {
                if (isUploading) {
                    isTimeOut = true;
                }
            } catch (IOException e) {
                System.out.println("connection lost!");
                e.printStackTrace();
                break;
            }
        }
    }

    private void parseMessage(String message) {
        String[] data = message.split(" ");
        switch (data[0]) {
            case "d":
                new Thread(() -> download(data[1], Integer.parseInt(data[2]), Integer.parseInt(data[3]))).start();
                break;
            case "u":
                new Thread(() -> upload(data[1], Integer.parseInt(data[2]))).start();
                break;
            case "ok":
                if (isUploading) ack = true;
                break;
            case "fileSent":
                System.out.println("download finished!");
                break;
            default:
                System.out.println(message);
        }
    }

    private void download(String filename, int fileSize, int chunkSize) {
        System.out.println("download started!");
        try {
            FileOutputStream fos = new FileOutputStream(filename);
            BufferedOutputStream bos = new BufferedOutputStream(fos);
            InputStream is = fileSocket.getInputStream();

            byte[] contents = new byte[chunkSize];
            int totalBytesRead = 0;
            while (totalBytesRead < fileSize) {
                int bytesRead = is.read(contents);
                bos.write(contents, 0, bytesRead);
                totalBytesRead += bytesRead;
            }
            bos.flush();
            fos.close();
        } catch (IOException e) {
            System.out.println("failed to download!");
        }
    }

    private void upload(String fileName, int chunkSize) {
        isUploading = true;
        System.out.println("upload started! chunk size: " + chunkSize);
        try {
            File file = new File(fileName);
            FileInputStream fis = new FileInputStream(file);
            BufferedInputStream bis = new BufferedInputStream(fis);
            OutputStream os = fileSocket.getOutputStream();
            int totalBytesSent = 0;
            byte[] chunk = new byte[chunkSize];
            ack = isTimeOut = false;
            while (totalBytesSent < file.length()) {
                int bytesSent = bis.read(chunk);
                os.write(chunk, 0, bytesSent);
                while (!ack && !isTimeOut) ;
                ack = false;
                if (isTimeOut) {
                    isTimeOut = false;
                    break;
                }
                totalBytesSent += bytesSent;
            }
            if (totalBytesSent == file.length()) {
                out.println("completed");
            } else {
                out.println("timeout");
                os.write("timeout".getBytes());
            }
            fis.close();
            os.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
        isUploading = false;
    }
}