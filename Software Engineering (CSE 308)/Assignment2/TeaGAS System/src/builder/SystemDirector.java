package builder;

import system.System_;

public class SystemDirector {

    public System_ construct(String packageName, String internetConnectionName, String frameworkName) {
        SystemBuilder systemBuilder = SystemBuilderFactory.createSystemBuilder(packageName);

        systemBuilder.buildController();
        systemBuilder.buildDisplay();
        systemBuilder.buildIdentification();
        systemBuilder.buildInternetConnection(internetConnectionName);
        systemBuilder.buildMicrocontroller();
        systemBuilder.buildStorage();
        systemBuilder.buildWebServer(frameworkName);
        systemBuilder.buildWeightMeasurement();

        return systemBuilder.getSystem();
    }
}
