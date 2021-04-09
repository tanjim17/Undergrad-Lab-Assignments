package pizza.decorator;

import pizza.Pizza;

abstract class Appetizer extends Decorator{
    Appetizer(Pizza pizza) { super(pizza); }
    public String getName() { return super.getName(); }
    public int getPrice() { return super.getPrice(); }
}
