package system.component.weightmeasurement;

public class WeightModule implements WeightMeasurement {
    public WeightModule() {
        System.out.println("WeightModule");
    }

    @Override
    public void getWeight() {
        System.out.println("weight module measuring weight...");
    }
}
