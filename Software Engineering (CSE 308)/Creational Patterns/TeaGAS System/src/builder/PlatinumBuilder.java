package builder;

import system.PlatinumSystem;
import system.component.weightmeasurement.WeightModule;

class PlatinumBuilder extends RaspberrySystemBuilder {
    PlatinumBuilder() {
        system = new PlatinumSystem();
    }

    @Override
    void buildWeightMeasurement() {
        system.setWeightMeasurement(new WeightModule());
    }
}
