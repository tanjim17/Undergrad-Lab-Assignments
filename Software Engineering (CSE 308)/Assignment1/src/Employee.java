import javafx.util.Pair;

import java.util.List;

abstract class Employee extends User {
    Employee(String name) {
        super(name);
    }

    void approveLoan(Bank bank) {
        List<Account> accounts = bank.getAccounts();
        List<Pair<String, Integer>> loanRequests = bank.getLoanRequests();
        for(Pair<String, Integer> loanRequest: loanRequests) {
            for(Account account: accounts) {
                String name = account.getName();
                if(name.equals(loanRequest.getKey())) {
                    int requestedLoan = loanRequest.getValue();
                    account.receiveLoan(requestedLoan);
                    System.out.println("Loan for " + name + " approved.");
                    break;
                }
            }
        }
        bank.clearLoanRequests();
    }

    void lookup(String name, Bank bank) {
        List<Account> accounts = bank.getAccounts();
        for(Account account: accounts) {
            if(account.getName().equals(name)) {
                System.out.println(name + "'s current balance is " + account.getBalance() + "$.");
                return;
            }
        }
    }

    void close() {System.out.println("Operation closed for " + this.getName() + ".");}

    void changeInterest(String accountType, double rate){System.out.println(errorMessage);}
    void seeInternalFund(Bank bank){System.out.println(errorMessage);}

    void deposit(int depositAmount) {System.out.println(errorMessage);}
    void withdraw(int withdrawAmount){System.out.println(errorMessage);}
    void query(){System.out.println(errorMessage);}
    void request(int requestedLoan, Bank bank){System.out.println(errorMessage);}
}
