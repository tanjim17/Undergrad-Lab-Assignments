import java.util.Scanner;

public class Client {
    public static void main(String[] args) {
        Editor editor = Editor.getEditor();
        String fileName;

        Scanner sc = new Scanner(System.in);
        while (true) {
            fileName = sc.next();
            if (fileName.equalsIgnoreCase("exit"))
                break;
            editor.setLanguageSettings(fileName);
        }
    }
}
