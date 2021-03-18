package builder;

import system.System_;

interface SystemBuilder {
    void buildController();
    void buildDisplay();
    void buildIdentification();
    void buildInternetConnection();
    void buildMicrocontroller();
    void buildStorage();
    void buildWebServer();
    void buildWeightMeasurement();

    System_ getSystem();
}
