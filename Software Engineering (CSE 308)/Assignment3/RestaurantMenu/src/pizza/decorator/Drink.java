package pizza.decorator;

import pizza.Pizza;

abstract class Drink extends Decorator{
    Drink(Pizza pizza) { super(pizza); }
    public String getName() { return super.getName(); }
    public int getPrice() { return super.getPrice(); }
}
