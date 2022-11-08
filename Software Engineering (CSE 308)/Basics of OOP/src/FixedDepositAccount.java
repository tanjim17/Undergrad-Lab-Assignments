class FixedDepositAccount extends Account {
    static final int MAX_ALLOWABLE_LOAN = 100000;
    static double INTEREST_RATE = 15;

    FixedDepositAccount(String name, int initialDeposit) {
        super(name, initialDeposit);
    }

    void deposit(int depositAmount) {
        if (depositAmount < 50000)
            System.out.println("at least 50,000$ should be deposited in fixed deposit account!");
        else {
            increaseBalance(depositAmount);
            System.out.println(depositAmount + " deposited. Current balance: " + getBalance() + "$");
        }
    }

    void withdraw(int withdrawAmount) {
        if(isBalanceExceeded(withdrawAmount)) return;
        if (getMaturityPeriod() < 1) {
            System.out.println("Withdrawing isn't allowed from fixed deposit account before 1 year has passed!");
        } else {
            decreaseBalance(withdrawAmount);
            System.out.println(withdrawAmount + "$ withdrawn. Current balance: " + getBalance() + "$.");
        }
    }
}
