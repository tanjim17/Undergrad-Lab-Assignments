package builder;

import system.GoldSystem;
import system.component.controller.Button;
import system.component.display.LEDDisplay;
import system.component.identification.RFID;
import system.component.microcontroller.Arduino;
import system.component.storage.SDCard;
import system.component.weightmeasurement.WeightModule;

class GoldBuilder extends SystemBuilder{
    GoldBuilder() {
        system = new GoldSystem();
    }
    @Override
    void buildMicroController() {
        system.setMicrocontroller(new Arduino());
    }

    @Override
    void buildWeightMeasurement() {
        system.setWeightMeasurement(new WeightModule());
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
        system.setDisplay(new LEDDisplay());
    }

    @Override
    void buildController() {
        system.setController(new Button());
    }
}
