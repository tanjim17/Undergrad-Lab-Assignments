import aesthetics.Aesthetics;
import factory.LanguageSettingsFactory;
import parser.Parser_;

class Editor {
    private static Editor editor;
    private Parser_ parser;
    private Aesthetics aesthetics;

    private Editor() {
    }

    static Editor getEditor() {
        if (editor == null) {
            editor = new Editor();
            System.out.println("Editor created.");
        }
        return editor;
    }

    void setLanguageSettings(String fileName) {
        LanguageSettingsFactory languageSettingsFactory = FactoryCreator.createLanguageSettingsFactory(fileName);
        if (languageSettingsFactory == null) {
            System.out.println("Unsupported file!");
        } else {
            parser = languageSettingsFactory.createParser();
            aesthetics = languageSettingsFactory.createAesthetics();
        }
    }
}
