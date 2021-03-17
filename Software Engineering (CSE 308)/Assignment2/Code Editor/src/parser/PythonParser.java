package parser;

public class PythonParser implements Parser_ {
    public PythonParser() {
        System.out.println("Python parser created.");
    }

    @Override
    public void parse() {
        System.out.println("Python parser is parsing file...");
    }
}
