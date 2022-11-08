package system.component.microcontroller;

import system.component.storage.Storage;

public class RaspberryPi implements MicroController, Storage {
    public RaspberryPi() {
        System.out.println("RaspberryPi (built-in storage)");
    }
}
