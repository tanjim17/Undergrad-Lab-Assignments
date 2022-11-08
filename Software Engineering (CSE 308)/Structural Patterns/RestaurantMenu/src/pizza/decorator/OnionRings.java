package pizza.decorator;

import pizza.Pizza;

public class OnionRings extends Appetizer {
    public OnionRings(Pizza pizza) { super(pizza); }
    public String getName() { return super.getName() + ", Onion Rings"; }
    public int getPrice() { return super.getPrice() + 100; }
}
