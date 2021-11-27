import java.io.*;
import java.net.Socket;
import java.util.*;

class ServerThread extends Thread {
    private Socket fileSocket;
    private BufferedReader in;
    private PrintWriter out;
    private User currUser;
    private static List<User> users = new ArrayList<>();
    private static List<Request> requests = new ArrayList<>();
    private final static int MAX_BUFFER_SIZE = (int) 2e8, MAX_CHUNK_SIZE = (int) 1e3, MIN_CHUNK_SIZE = (int) 5e2;
    private static int bufferSize = 0;
    private Thread receiveThread;
    private volatile boolean isReceiveCompleted;
    private volatile boolean isReceiveInterrupted;
    private volatile FileOutputStream currFileOutStream;
    private static Hashtable<Integer, Hashtable<Integer, FileInfo>> fileTable = new Hashtable<>();

    ServerThread(Socket socket, Socket fileSocket) {
        this.fileSocket = fileSocket;
        try {
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            out = new PrintWriter(socket.getOutputStream(), true);
        } catch (IOException e) {
            e.printStackTrace();
        }
        currUser = null;
        receiveThread = null;
        isReceiveCompleted = isReceiveInterrupted = false;
        currFileOutStream = null;
    }

    public void run() {
        while (true) {
            try {
                String message = in.readLine();
                parseMessage(message);
            } catch (IOException e) {
                if (currUser != null) {
                    logout();
                }
            }
        }
    }

    private void parseMessage(String message) {
        String[] data = message.split(" ");
        if (currUser == null) {
            login(data);
            return;
        }
        switch (data[0]) {
            case "ls":
                for (User user : users) {
                    out.println(user.getId() + " " + (user.isLoggedIn() ? "active" : "inactive"));
                }
                break;
            case "dir":
                int id = data.length == 1 ? currUser.getId() : Integer.parseInt(data[1]);
                Hashtable<Integer, FileInfo> table = fileTable.get(id);
                Enumeration<Integer> e = table.keys();
                while (e.hasMoreElements()) {
                    int fileId = e.nextElement();
                    FileInfo fileInfo = table.get(fileId);
                    if (id != currUser.getId() && fileInfo.getType().equals("private")) continue;
                    out.println(fileId + " " + fileInfo.getName() + " " + fileInfo.getType());
                }
                break;
            case "out":
                logout();
                break;
            case "d":
                int uploaderId = Integer.parseInt(data[1]);
                int fileId = Integer.parseInt(data[2]);
                FileInfo fileInfo = fileTable.get(uploaderId).get(fileId);
                if (fileInfo == null) break;
                if (fileInfo.getType().equals("private") && currUser.getId() != uploaderId) {
                    out.println("you can't download private files of others!");
                    break;
                }
                File file = new File("files/" + uploaderId + "/" + fileInfo.getType() +
                        "/" + fileInfo.getName());
                if (file.exists()) {
                    out.println("d " + fileInfo.getName() + " " + file.length() + " " + MAX_CHUNK_SIZE);
                    new Thread(() -> sendFile(file)).start();
                } else {
                    out.println("file doesn't exist!");
                }
                break;
            case "r":
                Request request = new Request(currUser.getId(), data[1]);
                requests.add(request);
                for (User user : users) {
                    if (user != currUser) user.receiveMessage(request.toString());
                }
                break;
            case "u":
                int fileSize = Integer.parseInt(data[4]);
                if (fileSize + bufferSize > MAX_BUFFER_SIZE) {
                    out.println("not enough space!");
                } else {
                    bufferSize += fileSize;
                    int chunkSize = (int) (Math.random() * (MAX_CHUNK_SIZE - MIN_CHUNK_SIZE)) + MIN_CHUNK_SIZE;
                    out.println("u " + data[1] + " " + chunkSize);
                    receiveThread = new Thread(() -> receiveFile(data[1], fileSize, chunkSize, data[2],
                            Integer.parseInt(data[3])));
                    receiveThread.start();
                }
                break;
            case "v":
                out.println(currUser.showUnreadMessages());
                break;
            case "timeout":
                if (receiveThread != null) isReceiveInterrupted = true;
                break;
            case "completed":
                if (receiveThread != null) isReceiveCompleted = true;
                break;
        }
    }

    private void login(String[] data) {
        if (!data[0].equals("l")) return;
        int id = Integer.parseInt(data[1]);
        for (User user : users) {
            if (user.getId() == id) {
                if (user.isLoggedIn()) out.println("already logged in!");
                else {
                    user.setLoginStatus(true);
                    currUser = user;
                    out.println("logged in!");
                }
                return;
            }
        }
        currUser = new User(id, true);
        users.add(currUser);
        fileTable.put(id, new Hashtable<>());
        String rootPath = "files/" + data[1];
        if (new File(rootPath + "/public").mkdirs() &&
                new File(rootPath + "/private").mkdir()) {
            out.println("account created!");
        } else out.println("account creation failed!");
    }

    private void logout() {
        System.out.println(currUser.getId() + " disconnected!");
        out.println("logged out!");
        currUser.setLoginStatus(false);
        currUser = null;
        if(currFileOutStream != null) { //logout in the middle of file transfer
            try {
                currFileOutStream.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private void sendFile(File file) {
        try {
            FileInputStream fis = new FileInputStream(file);
            BufferedInputStream bis = new BufferedInputStream(fis);
            OutputStream os = fileSocket.getOutputStream();
            int bytesSent = 0;
            byte[] chunk = new byte[MAX_CHUNK_SIZE];
            while (bytesSent < file.length()) {
                int chunkSize = bis.read(chunk);
                os.write(chunk, 0, chunkSize);
                bytesSent += chunkSize;
            }
            os.flush();
            fis.close();
            out.println("fileSent");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void receiveFile(String fileName, int fileSize, int chunkSize, String fileType, int reqId) {
        File file = new File("files/" + currUser.getId() + "/" + fileType + "/" + fileName);
        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(file);
            currFileOutStream = fos;
            BufferedOutputStream bos = new BufferedOutputStream(fos);
            InputStream is = fileSocket.getInputStream();

            byte[] contents = new byte[chunkSize];
            int totalBytesRead = 0;
            while (totalBytesRead < fileSize) {
                int bytesRead = is.read(contents);
                if (bytesRead == 7 && isReceiveInterrupted
                        && Arrays.equals(Arrays.copyOfRange(contents, 0, 7), "timeout".getBytes())) {
                    break;
                }
                bos.write(contents, 0, bytesRead);
                totalBytesRead += bytesRead;
//                try {
//                    Thread.sleep(31*1000);
//                } catch (InterruptedException e) {
//                    e.printStackTrace();
//                }
                out.println("ok");
            }
            bos.flush();
            fos.close();

            while (!isReceiveCompleted && !isReceiveInterrupted) ;
            if (isReceiveCompleted && totalBytesRead == fileSize) {
                out.println("upload finished!");
                Hashtable<Integer, FileInfo> table = fileTable.get(currUser.getId());
                Enumeration<Integer> e = table.keys();
                while (e.hasMoreElements()) {
                    if (fileName.equals(table.get(e.nextElement()).getName())) break;
                }
                if (!e.hasMoreElements()) table.put(table.size() + 1, new FileInfo(fileName, fileType));

                if (reqId != 0 && fileType.equals("public")) {
                    for (Request request : requests) {
                        if (reqId == request.getReqId()) {
                            for (User user : users) {
                                if (user.getId() == request.getSenderId()) {
                                    user.receiveMessage("requested file uploaded!" + "\nfile name: " +
                                            fileName + "\nuploader id: " + currUser.getId());
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
            } else cleanUp(file);
            isReceiveCompleted = isReceiveInterrupted = false;
        } catch (IOException e) {
            try {
                if (fos != null) {
                    fos.close();
                    cleanUp(file);
                }
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }
        bufferSize -= fileSize;
        currFileOutStream = null;
    }

    private void cleanUp(File file) {
        out.println("upload failed!");
        if (!file.delete()) System.out.println("couldn't remove file!");
    }
}
