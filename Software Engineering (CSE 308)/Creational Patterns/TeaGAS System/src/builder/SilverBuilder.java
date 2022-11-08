package builder;

import system.component.controller.Button;
import system.component.display.LCDDisplay;
import system.component.identification.RFID;
import system.component.microcontroller.ATMega32;
import system.SilverSystem;
import system.component.storage.SDCard;
import system.component.weightmeasurement.LoadSensor;

class SilverBuilder extends SystemBuilder {
    SilverBuilder() {
        system = new SilverSystem();
    }

    @Override
    void buildMicroController() {
        system.setMicrocontroller(new ATMega32());
    }

    @Override
    void buildWeightMeasurement() {
        system.setWeightMeasurement(new LoadSensor());
    }

    @Override
    void buildIdentification() {
        system.setIdentification(new RFID());
    }

    @Override
    void buildStorage() {
        system.setStorage(new SDCard());
    }

    @Override
    void buildDisplay() {
        system.setDisplay(new LCDDisplay());
    }

    @Override
    void buildController() {
        system.setController(new Button());
    }
}
