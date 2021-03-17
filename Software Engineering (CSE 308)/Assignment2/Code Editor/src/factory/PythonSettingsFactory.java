package factory;

import aesthetics.Aesthetics;
import aesthetics.Font_;
import aesthetics.PythonAesthetics;
import parser.Parser_;
import parser.PythonParser;

public class PythonSettingsFactory implements LanguageSettingsFactory{
    @Override
    public Parser_ createParser() {
        return new PythonParser();
    }

    @Override
    public Aesthetics createAesthetics() {
        return new PythonAesthetics(Font_.Consolas);
    }
}
