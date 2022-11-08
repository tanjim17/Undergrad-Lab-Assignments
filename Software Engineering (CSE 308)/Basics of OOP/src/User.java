abstract class User {
    private String name;
    static final String errorMessage = "You don’t have permission for this operation!";

    User(String name) {
        this.name = name;
    }

    String getName() {
        return name;
    }

    abstract void deposit(int depositAmount);
    abstract void withdraw(int withdrawAmount);
    abstract void query();
    abstract void request(int requestedLoan, Bank bank);

    abstract void approveLoan(Bank bank);
    abstract void changeInterest(String accountType, double rate);
    abstract void lookup(String name, Bank bank);
    abstract void seeInternalFund(Bank bank);

    abstract void close();
}
