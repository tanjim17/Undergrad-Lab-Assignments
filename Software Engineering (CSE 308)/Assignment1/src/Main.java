import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Bank bank = new Bank();
        User user = null;
        Scanner sc = new Scanner(System.in);
        while (true) {
            String[] input = sc.nextLine().split(" ");
            switch (input[0]) {
                case "Create":
                    if (input.length == 4) {
                        if(user != null) user.close();
                        user = bank.createAccount(input[1], input[2], Integer.parseInt(input[3]));
                    }
                    break;
                case "Open":
                    if (input.length == 2) {
                        if(user != null) user.close();
                        user = bank.login(input[1]);
                    }
                    break;
                case "Close":
                    if (user != null) {
                        user.close();
                        user = null;
                    }
                    break;
                case "Deposit":
                    if (input.length == 2 && user != null)
                        user.deposit(Integer.parseInt(input[1]));
                    break;
                case "Withdraw":
                    if (input.length == 2 && user != null)
                        user.withdraw(Integer.parseInt(input[1]));
                    break;
                case "Query":
                    if (user != null)
                        user.query();
                    break;
                case "Request":
                    if (input.length == 2 && user != null)
                        user.request(Integer.parseInt(input[1]), bank);
                    break;
                case "Approve":
                    if (input.length == 2 && input[1].equals("Loan") && user != null)
                        user.approveLoan(bank);
                    break;
                case "Change":
                    if (input.length == 3 && user != null)
                        user.changeInterest(input[1], Double.parseDouble(input[2]));
                    break;
                case "Lookup":
                    if (input.length == 2 && user != null)
                        user.lookup(input[1], bank);
                    break;
                case "See":
                    if (user != null)
                        user.seeInternalFund(bank);
                    break;
                case "INC":
                    bank.newYear();
                    break;
                case "Exit":
                    return;
                default:
                    System.out.println("Invalid command!");
            }
        }
    }
}
