package filesum;

public class FileSumAdapter extends FileIntegerSum{
    private FileAsciiSum fileAsciiSum;

    public FileSumAdapter() {
        fileAsciiSum = new FileAsciiSum();
    }

    public int calculateSum(String fileName) {
        return fileAsciiSum.calculateAsciiSum(fileName);
    }

}
