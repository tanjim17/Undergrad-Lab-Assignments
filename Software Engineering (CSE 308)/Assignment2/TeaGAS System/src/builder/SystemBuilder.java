package builder;

import system.System_;

interface SystemBuilder {
    void buildController();
    void buildDisplay();
    void buildIdentification();
    void buildInternetConnection(String internetConnectionName);
    void buildMicrocontroller();
    void buildStorage();
    void buildWebServer(String frameworkName);
    void buildWeightMeasurement();

    System_ getSystem();
}
