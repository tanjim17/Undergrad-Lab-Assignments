import javafx.util.Pair;

import java.util.ArrayList;
import java.util.List;

class Bank {
    private List<Employee> employees;
    private List<Account> accounts;
    private List<Pair<String, Integer>> loanRequests;
    private int internalFund;
    private int year;

    Bank() {
        employees = new ArrayList<>();
        employees.add(new ManagingDirector("MD"));
        for(int i=0; i<2; i++) {
            employees.add(new Officer("O"+(i+1)));
        }
        for(int i=0; i<5; i++) {
            employees.add(new Cashier("C"+(i+1)));
        }
        accounts = new ArrayList<>();
        loanRequests = new ArrayList<>();
        internalFund = 1000000;
        year = 0;
        System.out.println("Bank created. MD, O1, O2, C1, C2, C3, C4, C5 created.");
    }

    private User getUserIfExist(String name, List<? extends User> users) {
        for(User user: users) {
            if(user.getName().equals(name)) { return user; }
        }
        return null;
    }

    User createAccount(String name, String type, int initialDeposit) {
        if(getUserIfExist(name, accounts) != null || getUserIfExist(name, employees) != null) {
            System.out.println("Account name already exists!");
            return null;
        }

        Account account;
        switch (type) {
            case "Savings":
                account = new SavingsAccount(name, initialDeposit);
                break;
            case "Student":
                account = new StudentAccount(name, initialDeposit);
                break;
            case "FixedDeposit":
                if (initialDeposit < 100000) {
                    System.out.println("You need at least 100,000$ to open fixed deposit account!");
                    return null;
                }
                else
                    account = new FixedDepositAccount(name, initialDeposit);
                break;
            default:
                System.out.println("Invalid account type!");
                return null;
        }
        accounts.add(account);
        System.out.println(type + " account for "+ name +" created. Initial balance "+ initialDeposit +"$");
        return account;
    }

    User login(String name) {
        User user = getUserIfExist(name, employees);
        if(user != null) {
            System.out.print(name + " active. ");
            if(loanRequests.size() > 0 && !(user instanceof Cashier)) {
                System.out.print("There are loan approvals pending!");
            }
            System.out.println();
            return user;
        }
        user = getUserIfExist(name, accounts);
        if(user != null) {
            System.out.println("Welcome back, " + name + "!");
        }
        return user;
    }

    void newYear() {
        System.out.println(++year + " year(s) passed.");
        for(Account account: accounts) {
            account.increaseMaturityPeriod();
            account.addInterest();
            account.deductLoanInterest();
            account.deductServiceCharge();
        }
    }

    void receiveNewLoanRequest(String name, Integer requestedLoan) {
        loanRequests.add(new Pair<>(name, requestedLoan));
    }

    List<Account> getAccounts() {return accounts;}

    List<Pair<String, Integer>> getLoanRequests() {return loanRequests;}

    int getInternalFund() { return internalFund; }

    void clearLoanRequests() {loanRequests.clear();}
}
