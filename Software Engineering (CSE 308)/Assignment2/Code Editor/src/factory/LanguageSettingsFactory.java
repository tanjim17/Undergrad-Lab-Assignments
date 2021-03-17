package factory;

import aesthetics.Aesthetics;
import parser.Parser_;

public interface LanguageSettingsFactory {
    Parser_ createParser();
    Aesthetics createAesthetics();
}
