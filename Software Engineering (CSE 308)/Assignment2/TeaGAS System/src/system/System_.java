package system;

import system.component.controller.Controller;
import system.component.display.Display;
import system.component.identification.Identification;
import system.component.internetconnection.InternetConnection;
import system.component.microcontroller.MicroController;
import system.component.storage.Storage;
import system.component.webserver.WebServer;
import system.component.weightmeasurement.WeightMeasurement;

public abstract class System_ {
    Controller controller;
    Display display;
    Identification identification;
    InternetConnection internetConnection;
    MicroController microcontroller;
    Storage storage;
    WebServer webServer;
    WeightMeasurement weightMeasurement;

    public void setController(Controller controller) {
        this.controller = controller;
    }

    public void setDisplay(Display display) {
        this.display = display;
    }

    public void setIdentification(Identification identification) {
        this.identification = identification;
    }

    public void setInternetConnection(InternetConnection internetConnection) {
        this.internetConnection = internetConnection;
    }

    public void setMicrocontroller(MicroController microcontroller) {
        this.microcontroller = microcontroller;
    }

    public void setStorage(Storage storage) {
        this.storage = storage;
    }

    public void setWebServer(WebServer webServer) {
        this.webServer = webServer;
    }

    public void setWeightMeasurement(WeightMeasurement weightMeasurement) {
        this.weightMeasurement = weightMeasurement;
    }

    public void measureWeight() {
        weightMeasurement.getWeight();
    }
}
