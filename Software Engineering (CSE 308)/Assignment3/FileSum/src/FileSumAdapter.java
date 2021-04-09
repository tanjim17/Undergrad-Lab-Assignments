class FileSumAdapter extends FileIntegerSum{
    private FileAsciiSum fileAsciiSum;

    FileSumAdapter(FileAsciiSum fileAsciiSum) {
        this.fileAsciiSum = fileAsciiSum;
    }

    int calculateSum(String fileName) {
        return fileAsciiSum.calculateAsciiSum(fileName);
    }

}
