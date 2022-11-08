package pizza.decorator;

import pizza.Pizza;

public class Coke extends Drink{
    public Coke(Pizza pizza) { super(pizza); }
    public String getName() { return super.getName() + ", Coke"; }
    public int getPrice() { return super.getPrice() + 20; }
}
