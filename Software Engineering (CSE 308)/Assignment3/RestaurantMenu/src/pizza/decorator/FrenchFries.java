package pizza.decorator;

import pizza.Pizza;

public class FrenchFries extends Appetizer {
    public FrenchFries(Pizza pizza) { super(pizza); }
    public String getName() { return super.getName() + ", French Fries "; }
    public int getPrice() { return super.getPrice() + 100; }
}
