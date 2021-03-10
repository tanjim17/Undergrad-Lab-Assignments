class Cashier extends Employee{
    Cashier(String name) {
        super(name);
    }

    void approveLoan(Bank bank) {
        System.out.println("You don’t have permission for this operation!");
    }
}
