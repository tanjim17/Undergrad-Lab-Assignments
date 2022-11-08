package system.component.display;

import system.component.controller.Controller;

public class TouchScreen implements Display, Controller {
    public TouchScreen() {
        System.out.println("TouchScreen (display + controller)");
    }
}
