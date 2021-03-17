import factory.*;

abstract class FactoryCreator {
    static LanguageSettingsFactory createLanguageSettingsFactory(String fileName) {
        String extension = fileName.substring(fileName.lastIndexOf(".") + 1).toLowerCase();
        LanguageSettingsFactory languageSettingsFactory = null;

        switch (extension) {
            case "c":
                languageSettingsFactory = new CSettingsFactory();
                break;
            case "cpp":
                languageSettingsFactory = new CPPSettingsFactory();
                break;
            case "py":
                languageSettingsFactory = new PythonSettingsFactory();
        }
        return languageSettingsFactory;
    }
}
