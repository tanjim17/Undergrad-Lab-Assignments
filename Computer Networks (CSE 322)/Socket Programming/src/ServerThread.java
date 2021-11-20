import java.io.*;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

class ServerThread extends Thread {
    private Socket socket;
    //    private BufferedReader in;
//    private PrintWriter out;
    private ObjectInputStream in;
    private ObjectOutputStream out;
    private User currUser;
    private static List<User> users = new ArrayList<>();
    private static List<Request> requests = new ArrayList<>();
    private final static int MAX_BUFFER_SIZE = 1000000, MAX_CHUNK_SIZE = 10000, MIN_CHUNK_SIZE = 1000;
    private static int buffer_size = 0;

    ServerThread(Socket socket) {
        this.socket = socket;
        try {
            //in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            //out = new PrintWriter(socket.getOutputStream(), true);
            in = new ObjectInputStream(socket.getInputStream());
            out = new ObjectOutputStream(socket.getOutputStream());
        } catch (IOException e) {
            e.printStackTrace();
        }
        currUser = null;
    }

    public void run() {
        while (true) {
            try {
                String message = (String) in.readObject();
                parseMessage(message);
            } catch (IOException | ClassNotFoundException e) {
                System.out.println(currUser.getId() + " disconnected!");
                if (currUser != null)
                    logout();
                break;
            }
        }
    }

    private void parseMessage(String message) throws IOException {
        String[] data = message.split(" ");
        if (currUser == null) {
            login(data);
            return;
        }
        switch (data[0]) {
            case "ls":
                for (User user : users) {
                    out.writeObject(user.getId() + " " + user.isLoggedIn());
                }
                break;
            case "dir":
                String rootPathName = "files/" + (data.length == 1 ? currUser.getId() : data[1]) + "/";
                if (data.length == 1) {
                    out.writeObject("private:");
                    sendFileList(rootPathName + "private");
                }
                out.writeObject("public:");
                sendFileList(rootPathName + "public");
                break;
            case "out":
                logout();
                break;
            case "d":
                String path = "files/" + data[1];
                if (Integer.parseInt(data[1]) == currUser.getId() && data.length == 4 && data[3].equals("private")) {
                    path += "/private/";
                } else path += "/public/";
                path += data[2];
                File file = new File(path);
                if (file.exists()) {
                    out.writeObject("d " + data[2] + " " + file.length() + " " + MAX_CHUNK_SIZE);
                    sendFile(file);
                }
                break;
            case "r":
                Request request = new Request(currUser.getId(), data[1]);
                requests.add(request);
                for (User user : users) {
                    if (user != currUser) user.addUnreadMessage(request.toString());
                }
                break;
            case "u":
                int fileSize = Integer.parseInt(data[4]);
                if (fileSize + buffer_size > MAX_BUFFER_SIZE) {
                    out.writeObject("not enough space!");
                } else {
                    buffer_size += fileSize;
                    int chunkSize = (int) (Math.random() * (MAX_CHUNK_SIZE - MIN_CHUNK_SIZE)) + MIN_CHUNK_SIZE;
                    chunkSize = 500;
                    out.writeObject("u " + data[1] + " " + chunkSize);
                    receiveFile(data[1], fileSize, chunkSize, data[2], Integer.parseInt(data[3]));
                }
                break;
            case "v":
                out.writeObject(currUser.showUnreadMessages());
                break;
        }
    }

    private void login(String[] data) throws IOException {
        if (!data[0].equals("l")) return;
        int id = Integer.parseInt(data[1]);
        for (User user : users) {
            if (user.getId() == id) {
                if (user.isLoggedIn()) out.writeObject("already logged in!");
                else {
                    user.setServerThread(this);
                    currUser = user;
                }
                return;
            }
        }
        currUser = new User(id, this);
        users.add(currUser);
        String rootPath = "files/" + data[1];
        if (!new File(rootPath + "/public").mkdirs() ||
                !new File(rootPath + "/private").mkdir()) {
            out.writeObject("couldn't create directory!");
        }
    }

    private void logout() {
        currUser.setServerThread(null);
        currUser = null;
    }

    private void sendFile(File file) {
        try {
            FileInputStream fis = new FileInputStream(file);
            BufferedInputStream bis = new BufferedInputStream(fis);
            OutputStream os = socket.getOutputStream();
            long bytesSent = 0;
            byte[] chunk = new byte[MAX_CHUNK_SIZE];
            while (bytesSent < file.length()) {
                int chunkSize = bis.read(chunk);
                os.write(chunk, 0, chunkSize);
                System.out.println(chunkSize);
                bytesSent += chunkSize;
            }
            os.flush();
            fis.close();
            out.writeObject("file downloaded!");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void receiveFile(String fileName, int fileSize, int maxChunkSize, String fileType, int reqId) {
        System.out.println(fileSize + " " + maxChunkSize);
        File file = new File("files/" + currUser.getId() + "/" + fileType + "/" + fileName);
        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(file);
            BufferedOutputStream bos = new BufferedOutputStream(fos);
            InputStream is = socket.getInputStream();

            byte[] contents = new byte[maxChunkSize];
            int bytesRead = 0;
            while (bytesRead < fileSize) {
                int chunkSize = is.read(contents);
                System.out.println(chunkSize);
                if (chunkSize == 7 && Arrays.equals(Arrays.copyOfRange(contents, 0, 7), "timeout".getBytes())) {
                    break;
                }
                bos.write(contents, 0, chunkSize);
                bytesRead += chunkSize;
//                try {
//                    Thread.sleep(3000);
//                } catch (InterruptedException e) {
//                    e.printStackTrace();
//                }
                out.writeObject("ok");
            }
            bos.flush();
            fos.close();
            if (bytesRead < fileSize) cleanUp(file);
            int chunkSize = is.read(contents);
            System.out.println(chunkSize);
            if (chunkSize == 9 && Arrays.equals(Arrays.copyOfRange(contents, 0, 9), "completed".getBytes())) {
                if (bytesRead == fileSize) {
                    out.writeObject("file uploaded!");
                    if (reqId != 0 && fileType.equals("public")) {
                        for (Request request : requests) {
                            if (reqId == request.getReqId()) {
                                for (User user : users) {
                                    if (user.getId() == request.getSenderId()) {
                                        user.addUnreadMessage("requested file uploaded!" + "\nfile name: " +
                                                fileName + "\nuploader id: " + currUser.getId());
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                    }
                } else cleanUp(file);
            }
        } catch (IOException e) {
            try {
                if (fos != null) fos.close();
                cleanUp(file);
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }
        buffer_size -= fileSize;
    }

    private void sendFileList(String path) throws IOException {
        File[] files = new File(path).listFiles();
        if (files != null)
            for (File file : files) out.writeObject(file.getName());
    }

    private void cleanUp(File file) throws IOException {
        out.writeObject("uploading failed!");
        if (!file.delete()) System.out.println("couldn't remove file!");
    }
}
