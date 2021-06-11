import java.util.HashMap;
import java.util.LinkedList;

class JCC {
    private HashMap<Organization, LinkedList<Organization>> clientLists;
    private HashMap<String, Organization> providerList;

    JCC() {
        clientLists = new HashMap<>();
        providerList = new HashMap<>();
    }

    void setOrg(Organization organization, String service) {
        clientLists.put(organization, new LinkedList<>());
        providerList.put(service, organization);
    }

    void forwardService(Organization provider) {
        Organization client = clientLists.get(provider).remove();
        client.receive(provider.getClass().getSimpleName() + " serves the request of "
                + client.getClass().getSimpleName());
    }

    void forwardRequest(Organization client, String service) {
        clientLists.get(providerList.get(service)).add(client);
        System.out.println(client.getClass().getSimpleName() + " requests for " + service + " service");
    }
}
