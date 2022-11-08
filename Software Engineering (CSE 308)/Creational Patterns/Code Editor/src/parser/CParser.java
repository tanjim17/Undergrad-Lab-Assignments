package parser;

public class CParser implements Parser_ {
    public CParser() {
        System.out.println("C parser created.");
    }

    @Override
    public void parse() {
        System.out.println("C parser is parsing file...");
    }
}
