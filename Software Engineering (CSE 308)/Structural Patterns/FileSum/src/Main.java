import filesum.FileIntegerSum;
import filesum.FileSumAdapter;
import java.util.Scanner;

public class Main {
    public static void main(String [] args) {
        Scanner scanner = new Scanner(System.in);
        String option;
        String fileName;
        FileIntegerSum fileIntegerSum = new FileIntegerSum();
        FileIntegerSum fileSumAdapter = new FileSumAdapter();
        while(true) {
            System.out.println("Which sum do you want?\n1.Integer\n2.ASCII\nPress any other key to exit");
            option = scanner.nextLine();
            if(!option.equals("1") && !option.equals("2")) {
                break;
            } else {
                System.out.println("Enter filename:");
                fileName = scanner.nextLine();
                if(option.equals("1")) {
                    System.out.println("Integer Sum: " + fileIntegerSum.calculateSum(fileName) + "\n");
                } else {
                    System.out.println("Ascii Sum: " + fileSumAdapter.calculateSum(fileName) + "\n");
                }
            }
        }
    }
}