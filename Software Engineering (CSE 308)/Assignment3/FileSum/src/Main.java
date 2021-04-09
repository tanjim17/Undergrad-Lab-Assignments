import java.util.Scanner;

public class Main {
    public static void main(String [] args) {
        Scanner scanner = new Scanner(System.in);
        String fileName = scanner.nextLine();

        FileAsciiSum fileAsciiSum = new FileAsciiSum();
        FileIntegerSum fileSumAdapter = new FileSumAdapter(fileAsciiSum);

        System.out.println("Ascii Sum: " + fileSumAdapter.calculateSum(fileName));
    }
}
