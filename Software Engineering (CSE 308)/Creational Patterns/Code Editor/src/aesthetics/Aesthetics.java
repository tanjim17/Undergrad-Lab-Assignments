package aesthetics;

public abstract class Aesthetics {
    private Font_ font;
    private Color_ color;
    private Style_ style;

    Aesthetics(Font_ font) {
        this.font = font;
        color = Color_.blue;
        style = Style_.Normal;
    }
}
