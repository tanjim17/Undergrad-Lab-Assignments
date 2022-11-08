package builder;

import system.component.display.TouchScreen;
import system.component.identification.NFC;
import system.component.microcontroller.RaspberryPi;
import system.component.weightmeasurement.LoadSensor;

abstract class RaspberrySystemBuilder extends SystemBuilder{
@Override
    void buildMicroController() {
        system.setMicrocontroller(new RaspberryPi());
    }

    @Override
    void buildStorage() {
        //raspberry pi got built-in storage
    }

    @Override
    void buildWeightMeasurement() {
        system.setWeightMeasurement(new LoadSensor());
    }

    @Override
    void buildIdentification() {
        system.setIdentification(new NFC());
    }

    @Override
    void buildController() {
        //touch screen works as controller
    }

    @Override
    void buildDisplay() {
        system.setDisplay(new TouchScreen());
    }
}
