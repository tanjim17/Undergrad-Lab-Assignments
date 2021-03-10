class StudentAccount extends Account {
    static final int MAX_ALLOWABLE_LOAN = 1000;
    static double INTEREST_RATE = 5;

    StudentAccount(String name, int initialDeposit) {
        super(name, initialDeposit);
    }

    void withdraw(int withdrawAmount) {
        if(isBalanceExceeded(withdrawAmount)) return;
        if (withdrawAmount > 10000) {
            System.out.println("Student can't withdraw more than 10,000$!");
        } else {
            decreaseBalance(withdrawAmount);
            System.out.println(withdrawAmount + "$ withdrawn. Current balance: " + getBalance() + "$.");
        }
    }

    void deductServiceCharge() {}
}
