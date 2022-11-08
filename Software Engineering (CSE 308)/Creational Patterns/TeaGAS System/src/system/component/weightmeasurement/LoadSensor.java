package system.component.weightmeasurement;

public class LoadSensor implements WeightMeasurement {
    public LoadSensor() {
        System.out.println("LoadSensor");
    }

    @Override
    public void getWeight() {
        System.out.println("load sensor measuring weight...");
    }
}
