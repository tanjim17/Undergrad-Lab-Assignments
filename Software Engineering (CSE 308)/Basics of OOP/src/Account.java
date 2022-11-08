abstract class Account extends User{
    private double balance;
    private int maturityPeriod;
    private int loan;

    Account(String name, int initialDeposit) {
        super(name);
        balance = initialDeposit;
        maturityPeriod = 0;
        loan = 0;
    }

    double getBalance() {
        return balance;
    }

    int getMaturityPeriod() {
        return maturityPeriod;
    }

    boolean isBalanceExceeded(int amount) {
        if (balance < amount) {
            System.out.println("Not enough balance!");
            return true;
        }
        return false;
    }

    void increaseBalance(double amount) {
        balance += amount;
    }

    void decreaseBalance(double amount) {
        balance -= amount;
    }

    void deposit(int depositAmount) {
        increaseBalance(depositAmount);
        System.out.println(depositAmount + "$ deposited. Current balance: " + balance + "$");
    }

    void query() {
        System.out.print("Current balance: " + balance + "$");
        if(loan > 0)
            System.out.print(", Loan: " + loan + "$");
        System.out.println();
    }

    void request(int requestedLoan, Bank bank) {
        int maxAllowableLoan = 0;
        if(this instanceof SavingsAccount) maxAllowableLoan = SavingsAccount.MAX_ALLOWABLE_LOAN;
        if(this instanceof StudentAccount) maxAllowableLoan = StudentAccount.MAX_ALLOWABLE_LOAN;
        if(this instanceof FixedDepositAccount) maxAllowableLoan = FixedDepositAccount.MAX_ALLOWABLE_LOAN;
        if(requestedLoan > maxAllowableLoan) {
            System.out.println("You can't request for loan more than "+ maxAllowableLoan +"$!");
        } else {
            bank.receiveNewLoanRequest(this.getName(), requestedLoan);
            System.out.println("Loan request successful, sent for approval!");
        }
    }

    void receiveLoan(int loan) {
        this.loan += loan;
        increaseBalance(loan);
    }

    void addInterest() {
        double interestRate = 0;
        if(this instanceof SavingsAccount) interestRate = SavingsAccount.INTEREST_RATE;
        if(this instanceof StudentAccount) interestRate = StudentAccount.INTEREST_RATE;
        if(this instanceof FixedDepositAccount) interestRate = FixedDepositAccount.INTEREST_RATE;
        increaseBalance(balance*interestRate/100);
    }

    void deductLoanInterest() {decreaseBalance(loan*0.1);}

    void deductServiceCharge() {decreaseBalance(500);}

    void increaseMaturityPeriod() {maturityPeriod ++;}

    void close() { System.out.println("Transaction closed for " + this.getName() + "."); }

    void approveLoan(Bank bank){System.out.println(errorMessage);}
    void changeInterest(String accountType, double rate){System.out.println(errorMessage);}
    void lookup(String name, Bank bank){System.out.println(errorMessage);}
    void seeInternalFund(Bank bank){System.out.println(errorMessage);}
}
