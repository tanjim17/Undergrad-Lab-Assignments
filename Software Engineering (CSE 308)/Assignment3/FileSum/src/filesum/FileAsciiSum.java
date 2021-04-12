package filesum;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;

class FileAsciiSum {
    int calculateAsciiSum(String fileName) {
        File file = new File(fileName);
        int sum = 0;
        try (FileReader fr = new FileReader(file)) {
            int character;
            while ((character = fr.read()) != -1) {
                if(character != ' ')
                    sum += character;
            }
        } catch (IOException e) {
            System.err.println("I/O error!");
        }
        return sum;
    }
}
