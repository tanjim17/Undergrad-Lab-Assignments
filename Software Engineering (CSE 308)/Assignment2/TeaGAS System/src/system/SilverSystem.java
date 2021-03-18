package system;

import system.component.controller.Controller;
import system.component.display.Display;
import system.component.identification.Identification;
import system.component.internetconnection.InternetConnection;
import system.component.microcontroller.Microcontroller;
import system.component.storage.Storage;
import system.component.webserver.WebServer;
import system.component.weightmeasurement.WeightMeasurement;

public class SilverSystem implements System_ {
    private Controller controller;
    private Display display;
    private Identification identification;
    private InternetConnection internetConnection;
    private Microcontroller microcontroller;
    private Storage storage;
    private WebServer webServer;
    private WeightMeasurement weightMeasurement;

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

    public void setMicrocontroller(Microcontroller microcontroller) {
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

    public SilverSystem() {
        System.out.println("SilverSystem created.");
    }
}
