import java.util.ArrayList;

class Stock {
    private String name;
    private int count;
    private float value;
    private ArrayList<User> subscribers;

    Stock(String name, int count, float value) {
        this.name = name;
        this.count = count;
        this.value = value;
        subscribers = new ArrayList<>();
    }

    private void notifyUsers(String property) {
        for (User subscriber : subscribers) {
            subscriber.update(this, property);
        }
    }

    void subscribe(User user) {
        subscribers.add(user);
    }

    void unsubscribe(User user) {
        subscribers.remove(user);
    }

    String getName() {
        return name;
    }

    int getCount() {
        return count;
    }

    void updateCount(int count) {
        this.count += count;
        notifyUsers("count");
    }

    float getValue() {
        return value;
    }

    void updateValue(float value) {
        this.value += value;
        notifyUsers("value");
    }
}
