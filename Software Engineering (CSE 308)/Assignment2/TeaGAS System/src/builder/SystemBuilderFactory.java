package builder;

import builder.*;

public class SystemBuilderFactory {
    static SystemBuilder createSystemBuilder(String packageName) {
        SystemBuilder systemBuilder = null;

        switch (packageName.toLowerCase()) {
            case "silver":
                systemBuilder = new SilverBuilder();
                break;
            case "gold":
                systemBuilder = new GoldBuilder();
                break;
            case "diamond":
                systemBuilder = new DiamondBuilder();
                break;
            case "platinum":
                systemBuilder = new PlatinumBuilder();
                break;
        }
        return systemBuilder;
    }
}
