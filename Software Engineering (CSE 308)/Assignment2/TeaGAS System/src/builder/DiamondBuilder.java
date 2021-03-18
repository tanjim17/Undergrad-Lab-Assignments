package builder;

import system.DiamondSystem;
import system.component.weightmeasurement.LoadSensor;

class DiamondBuilder extends RaspberrySystemBuilder{
    DiamondBuilder() {
        system = new DiamondSystem();
    }

    @Override
    void buildWeightMeasurement() {
        system.setWeightMeasurement(new LoadSensor());
    }
}
