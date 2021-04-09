import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

class FileIntegerSum {
    int calculateSum(String fileName) {
        Scanner scanner;
        try {
            scanner = new Scanner(new File(fileName));
        } catch (FileNotFoundException e) {
            System.err.println("File not found!");
            return 0;
        }
        int sum = 0;
        while(scanner.hasNextInt())
            sum += scanner.nextInt();
        return sum;
    }
}
