package factory;

import aesthetics.Aesthetics;
import aesthetics.CPPAesthetics;
import aesthetics.Font_;
import parser.CPPParser;
import parser.Parser_;

public class CPPSettingsFactory implements LanguageSettingsFactory{
    @Override
    public Parser_ createParser() {
        return new CPPParser();
    }

    @Override
    public Aesthetics createAesthetics() {
        return new CPPAesthetics(Font_.Monaco);
    }
}
