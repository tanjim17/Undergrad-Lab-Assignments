class SavingsAccount extends Account{
    static final int MAX_ALLOWABLE_LOAN = 10000;
    static double INTEREST_RATE = 10;

    SavingsAccount(String name, int initialDeposit) {
        super(name, initialDeposit);
    }

    void withdraw(int withdrawAmount) {
        if(isBalanceExceeded(withdrawAmount)) return;
        if(getBalance() - withdrawAmount < 1000) {
            System.out.println("Balance can't be less 1000$ in a savings account!");
        }
        else {
            decreaseBalance(withdrawAmount);
            System.out.println(withdrawAmount + "$ withdrawn. Current balance: " + getBalance() + "$.");
        }
    }
}
