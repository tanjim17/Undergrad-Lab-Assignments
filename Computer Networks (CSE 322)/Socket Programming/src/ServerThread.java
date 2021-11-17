import java.io.*;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

class ServerThread extends Thread {
    private Socket socket;
    private BufferedReader in;
    private PrintWriter out;
    private User currUser;
    private static List<User> users = new ArrayList<>();

    ServerThread(Socket socket) {
        this.socket = socket;
        try {
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            out = new PrintWriter(socket.getOutputStream(), true);
        } catch (IOException e) {
            e.printStackTrace();
        }
        currUser = null;
    }

    public void run() {
        while (true) {
            try {
                String message = in.readLine();
                parseMessage(message);
            } catch (IOException e) {
                currUser.setLoginStatus(false);
                break;
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
                for (User user : users) out.println(user.getId() + " " + user.isLoggedIn());
                break;
            case "dir":
                try {
                    Files.walk(Paths.get("files/" + currUser.getId()))
                            .filter(Files::isRegularFile).forEach(out::println);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                break;
            case "see":
                File[] dir = new File("files/" + data[1] + "/public").listFiles();
                if (dir != null)
                    for (File file : dir) out.println(file.getName());
                break;
            case "o":
                currUser.setLoginStatus(false);
                currUser = null;
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
                }
                return;
            }
        }
        currUser = new User(id, true);
        users.add(currUser);
        String rootPath = "files/" + data[1];
        if (!new File(rootPath + "/public").mkdirs() ||
                !new File(rootPath + "/private").mkdir()) {
            out.println("couldn't create directory!");
        }
    }
}
