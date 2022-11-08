package pizza.decorator;
import pizza.Pizza;

abstract class Decorator implements Pizza {
    private Pizza pizza;

    Decorator(Pizza pizza) { this.pizza = pizza; }
    public String getName() { return pizza.getName(); }
    public int getPrice() { return pizza.getPrice(); }
}
