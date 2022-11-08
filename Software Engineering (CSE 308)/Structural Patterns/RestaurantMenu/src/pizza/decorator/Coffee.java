package pizza.decorator;

import pizza.Pizza;

public class Coffee extends Drink{
    public Coffee(Pizza pizza) { super(pizza); }
    public String getName() { return super.getName() + ", Coffee"; }
    public int getPrice() { return super.getPrice() + 40; }
}
