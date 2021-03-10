class ManagingDirector extends Employee {
    ManagingDirector(String name) {
        super(name);
    }

    void changeInterest(String accountType, double rate) {
        switch (accountType) {
            case "Savings":
                SavingsAccount.INTEREST_RATE = rate;
                break;
            case "Student":
                StudentAccount.INTEREST_RATE = rate;
                break;
            case "FixedDeposit":
                FixedDepositAccount.INTEREST_RATE = rate;
                break;
            default:
                System.out.println("Account type is invalid!");
                return;
        }
        System.out.println(accountType + " account's interest rate has been changed!");
    }

    void seeInternalFund(Bank bank) {
        System.out.println("Current internal fund is " + bank.getInternalFund() + "$.");
    }
}

