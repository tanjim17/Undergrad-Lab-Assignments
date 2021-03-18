package builder;

import system.System_;
import system.component.internetconnection.*;
import system.component.webserver.*;

abstract class SystemBuilder {
    System_ system;

    abstract void buildController();
    abstract void buildDisplay();
    abstract void buildIdentification();
    abstract void buildMicroController();
    abstract void buildStorage();
    abstract void buildWeightMeasurement();

    void buildInternetConnection(String internetConnectionName) {
        InternetConnection internetConnection = null;
        switch (internetConnectionName) {
            case "Wifi":
                internetConnection = new Wifi();
                break;
            case "Ethernet":
                internetConnection = new Ethernet();
                break;
            case "GSM":
                internetConnection = new GSM();
                break;
        }
        system.setInternetConnection(internetConnection);
    }

    void buildWebServer(String frameworkName) {
        WebServer webServer = null;
        switch (frameworkName) {
            case "Django":
                webServer = new DjangoServer();
                break;
            case "Spring":
                webServer = new SpringServer();
                break;
            case "Laravel":
                webServer = new LaravelServer();
                break;
        }
        system.setWebServer(webServer);
    }

    System_ getSystem() {return system;}
}
