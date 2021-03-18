package builder;

import system.component.controller.Button;
import system.component.display.LCDDisplay;
import system.component.identification.RFID;
import system.component.microcontroller.ATMega32;
import system.SilverSystem;
import system.System_;
import system.component.storage.SDCard;
import system.component.weightmeasurement.LoadSensor;

class SilverBuilder implements SystemBuilder{
    private SilverSystem silverSystem;

    SilverBuilder() {
        silverSystem = new SilverSystem();
    }

    @Override
    public void buildMicrocontroller() {
        silverSystem.setMicrocontroller(new ATMega32());
    }

    @Override
    public void buildWeightMeasurement() {
        silverSystem.setWeightMeasurement(new LoadSensor());
    }

    @Override
    public void buildIdentification() {
        silverSystem.setIdentification(new RFID());
    }

    @Override
    public void buildStorage() {
        silverSystem.setStorage(new SDCard());
    }

    @Override
    public void buildWebServer(String frameworkName) {

    }

    @Override
    public void buildDisplay() {
        silverSystem.setDisplay(new LCDDisplay());
    }

    @Override
    public void buildInternetConnection(String internetConnectionName) {

    }

    @Override
    public void buildController() {
        silverSystem.setController(new Button());
    }

    @Override
    public System_ getSystem() {
        return silverSystem;
    }
}
