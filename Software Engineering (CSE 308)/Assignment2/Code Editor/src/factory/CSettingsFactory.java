package factory;

import aesthetics.Aesthetics;
import aesthetics.CAesthetics;
import aesthetics.Font_;
import parser.CParser;
import parser.Parser_;

public class CSettingsFactory implements LanguageSettingsFactory {
    @Override
    public Parser_ createParser() { return new CParser(); }

    @Override
    public Aesthetics createAesthetics() {
        return new CAesthetics(Font_.Courier_New);
    }
}
