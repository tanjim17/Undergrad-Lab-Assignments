package sample;

import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;

import java.beans.SimpleBeanInfo;

public class Words {

    private final SimpleStringProperty word;
    private final SimpleIntegerProperty frequency;


    public Words(String word, int frequency) {
        this.word = new SimpleStringProperty(word);
        this.frequency = new SimpleIntegerProperty(frequency);
    }

    public String getWord() {
        return word.get();
    }

    public void setWord(String word) {
        this.word.set(word);
    }

    public int getFrequency() {
        return frequency.get();
    }

    public void setFrequency(int frequency) {
        this.frequency.set(frequency);
    }
}
