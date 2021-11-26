class FileInfo {
    private String name;
    private String type;

    FileInfo(String name, String type) {
        this.name = name;
        this.type = type;
    }

    String getName() {
        return name;
    }

    String getType() {
        return type;
    }
}