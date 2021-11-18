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
                if(currUser != null)
                    logout();
                break;
            }
        }
    }

    private void parseMessage(String message) throws IOException{
        String[] data = message.split(" ");
        if (currUser == null) {
            login(data);
            out.writeObject(currUser.showUnreadMessages());
            return;
        }
        switch (data[0]) {
            case "ls":
                for (User user : users) {
                    out.writeObject(user.getId() + " " + user.isLoggedIn());
                }
                break;
            case "dir":
                String rootPathName = "files/" + currUser.getId() + "/";
                out.writeObject("private:");
                sendFileList(rootPathName + "private");
                out.writeObject("public:");
                sendFileList(rootPathName + "public");
                break;
            case "see":
                sendFileList("files/" + data[1] + "/public");
                break;
            case "out":
                logout();
                break;
            case "d":
                String path = "files/" + data[1];
                if(Integer.parseInt(data[1]) == currUser.getId() && data.length == 4 && data[3].equals("pvt")) {
                    path += "/private/";
                } else path += "/public/";
                path += data[2];
                File file = new File( path);
                if(file.exists()) {
                    out.writeObject("d " + data[2] + " " + file.length() + " " + MAX_CHUNK_SIZE);
                    transferFile(file);
                }
                break;
            case "r":
                Request request = new Request(currUser.getId(), data[1]);
                requests.add(request);
                for (User user : users) {
                    if(user == currUser) continue;
                    if(user.isLoggedIn())
                        user.getServerThread().getObjectOutputStream().writeObject(request.toString());
                    else
                        user.addUnreadMessage(request.toString());
                }

        }
    }

    private void login(String[] data) throws IOException{
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

    private void transferFile(File file) {
        try {
            BufferedInputStream bis = new BufferedInputStream(new FileInputStream(file));
            OutputStream os = socket.getOutputStream();
            long bytesSent = 0;
            byte[] chunk = new byte[MAX_CHUNK_SIZE];
            while (bytesSent < file.length()) {
                int chunkSize = bis.read(chunk);
                os.write(chunk, 0, chunkSize);
                System.out.println(chunkSize);
                //out.writeObject(Arrays.copyOfRange(chunk, 0, chunkSize));
                bytesSent += chunkSize;
            }
            os.flush();
            //out.flush();
            System.out.println("file sent successfully!");
        } catch (IOException e) {
            System.err.println("couldn't transfer file!");
        }
    }

    private ObjectOutputStream getObjectOutputStream() {
        return out;
    }

    private void sendFileList(String path) throws IOException{
        File[] files = new File(path).listFiles();
        if (files != null)
            for (File file : files) out.writeObject(file.getName());
    }
}
