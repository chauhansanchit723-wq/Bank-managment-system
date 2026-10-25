
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <cstdlib>
using namespace std;

const double SAVINGS_MIN_BALANCE = 500.0;
const double CURRENT_OVERDRAFT_LIMIT = 5000.0;

int readInt(const string& prompt) {
    string line;
    int value;
    char extra;

    while (true) {
        cout << prompt;
        if (!getline(cin, line)) {
            cout << "\nInput ended. Exiting program.\n";
            exit(0);
        }

        stringstream ss(line);
        if (ss >> value && !(ss >> extra) && value > 0)
            return value;

        cout << "Invalid input! Please enter a positive whole number.\n";
    }
}

double readAmount(const string& prompt) {
    string line;
    double value;
    char extra;

    while (true) {
        cout << prompt;
        if (!getline(cin, line)) {
            cout << "\nInput ended. Exiting program.\n";
            exit(0);
        }

        stringstream ss(line);
        if (ss >> value && !(ss >> extra) &&
            value > 0 && value <= 10000000) {
            double paise = value * 100;
            if (fabs(paise - floor(paise + 0.5)) < 0.000001)
                return value;
        }

        cout << "Invalid amount! Enter a positive amount "
             << "(maximum 2 decimal places).\n";
    }
}

string readText(const string& prompt) {
    string line;

    while (true) {
        cout << prompt;
        if (!getline(cin, line)) {
            cout << "\nInput ended. Exiting program.\n";
            exit(0);
        }

        if (line.find_first_not_of(" \t") != string::npos)
            return line;

        cout << "Invalid input! This field cannot be empty.\n";
    }
}

void printTableHeader() {
    cout << left
         << setw(9) << "Acc No"
         << setw(22) << "Holder Name"
         << setw(10) << "Type"
         << setw(14) << "Balance (Rs.)" << endl;
    cout << string(55, '-') << endl;
}

// Abstract base class
class Account {
protected:
    int accountNumber;
    string holderName;
    double balance;

public:
    Account(int number, string name, double openingBalance) {
        accountNumber = number;
        holderName = name;
        balance = openingBalance;
    }

    virtual ~Account() {}

    int getAccountNumber() const {
        return accountNumber;
    }

    string getHolderName() const {
        return holderName;
    }

    double getBalance() const {
        return balance;
    }

    void deposit(double amount) {
        balance += amount;
    }

    virtual bool withdraw(double amount) = 0;
    virtual string getAccountType() const = 0;

    void displayDetails() const {
        cout << left
             << setw(9) << accountNumber
             << setw(22) << holderName
             << setw(10) << getAccountType()
             << setw(14) << balance << endl;
    }
};

// Derived class: Savings Account
class SavingsAccount : public Account {
public:
    SavingsAccount(int number, string name, double openingBalance)
        : Account(number, name, openingBalance) {}

    bool withdraw(double amount) override {
        if (balance - amount < SAVINGS_MIN_BALANCE)
            return false;

        balance -= amount;
        return true;
    }

    string getAccountType() const override {
        return "Savings";
    }
};

// Derived class: Current Account
class CurrentAccount : public Account {
public:
    CurrentAccount(int number, string name, double openingBalance)
        : Account(number, name, openingBalance) {}

    bool withdraw(double amount) override {
        if (balance - amount < -CURRENT_OVERDRAFT_LIMIT)
            return false;

        balance -= amount;
        return true;
    }

    string getAccountType() const override {
        return "Current";
    }
};

// Bank management class
class Bank {
private:
    vector<Account*> accounts;
    int nextAccountNumber;

    int findIndex(int accNo) const {
        for (size_t i = 0; i < accounts.size(); i++) {
            if (accounts[i]->getAccountNumber() == accNo)
                return static_cast<int>(i);
        }
        return -1;
    }

public:
    Bank() {
        nextAccountNumber = 1001;
    }

    ~Bank() {
        for (size_t i = 0; i < accounts.size(); i++)
            delete accounts[i];
    }

    void createAccount() {
        cout << "Select account type:\n";
        cout << "1. Savings Account (minimum balance Rs. 500.00)\n";
        cout << "2. Current Account (overdraft up to Rs. 5000.00)\n";

        int type = readInt("Enter type (1 or 2): ");

        if (type != 1 && type != 2) {
            cout << "Invalid account type!\n";
            return;
        }

        string name = readText("Enter Account Holder Name: ");
        double opening = readAmount("Enter Opening Deposit (Rs.): ");

        if (type == 1 && opening < SAVINGS_MIN_BALANCE) {
            cout << "A Savings account needs an opening deposit of at least "
                 << "Rs. " << SAVINGS_MIN_BALANCE << ".\n";
            return;
        }

        Account* newAccount;

        if (type == 1)
            newAccount = new SavingsAccount(
                nextAccountNumber, name, opening);
        else
            newAccount = new CurrentAccount(
                nextAccountNumber, name, opening);

        accounts.push_back(newAccount);

        cout << "Account created successfully! Account Number: "
             << nextAccountNumber << endl;

        nextAccountNumber++;
    }

    void depositMoney() {
        int accNo = readInt("Enter Account Number: ");
        int index = findIndex(accNo);

        if (index == -1) {
            cout << "No account found with number "
                 << accNo << ".\n";
            return;
        }

        double amount = readAmount("Enter Amount to Deposit (Rs.): ");
        accounts[index]->deposit(amount);

        cout << "Rs. " << amount << " deposited successfully. "
             << "New balance: Rs. "
             << accounts[index]->getBalance() << endl;
    }

    void withdrawMoney() {
        int accNo = readInt("Enter Account Number: ");
        int index = findIndex(accNo);

        if (index == -1) {
            cout << "No account found with number "
                 << accNo << ".\n";
            return;
        }

        double amount = readAmount("Enter Amount to Withdraw (Rs.): ");

        if (accounts[index]->withdraw(amount)) {
            cout << "Rs. " << amount << " withdrawn successfully. "
                 << "New balance: Rs. "
                 << accounts[index]->getBalance() << endl;
        } else {
            cout << "Withdrawal failed! Not enough balance for a "
                 << accounts[index]->getAccountType()
                 << " account.\n";
        }
    }

    void checkAccount() const {
        int accNo = readInt("Enter Account Number: ");
        int index = findIndex(accNo);

        if (index == -1) {
            cout << "No account found with number "
                 << accNo << ".\n";
            return;
        }

        cout << "Account found:\n";
        printTableHeader();
        accounts[index]->displayDetails();
    }

    void displayAllAccounts() const {
        if (accounts.empty()) {
            cout << "No accounts have been created yet.\n";
            return;
        }

        double totalMoney = 0;

        printTableHeader();

        for (size_t i = 0; i < accounts.size(); i++) {
            accounts[i]->displayDetails();
            totalMoney += accounts[i]->getBalance();
        }

        cout << string(55, '-') << endl;
        cout << "Total accounts: " << accounts.size()
             << " | Total balance: Rs. "
             << totalMoney << endl;
    }

    void transferMoney() {
        int fromNo = readInt("Enter Sender Account Number: ");
        int fromIndex = findIndex(fromNo);

        if (fromIndex == -1) {
            cout << "No account found with number "
                 << fromNo << ".\n";
            return;
        }

        int toNo = readInt("Enter Receiver Account Number: ");
        int toIndex = findIndex(toNo);

        if (toIndex == -1) {
            cout << "No account found with number "
                 << toNo << ".\n";
            return;
        }

        if (fromNo == toNo) {
            cout << "Sender and receiver accounts cannot be the same!\n";
            return;
        }

        double amount = readAmount("Enter Amount to Transfer (Rs.): ");

        if (accounts[fromIndex]->withdraw(amount)) {
            accounts[toIndex]->deposit(amount);

            cout << "Rs. " << amount
                 << " transferred from account " << fromNo
                 << " to account " << toNo
                 << " successfully!\n";
        } else {
            cout << "Transfer failed! Not enough balance for a "
                 << accounts[fromIndex]->getAccountType()
                 << " account.\n";
        }
    }
};

void showMenu() {
    cout << "\n===== BANK MANAGEMENT SYSTEM =====\n";
    cout << "1. Create New Account\n";
    cout << "2. Deposit Money\n";
    cout << "3. Withdraw Money\n";
    cout << "4. Check Account Details\n";
    cout << "5. Display All Accounts\n";
    cout << "6. Transfer Money\n";
    cout << "7. Exit\n";
    cout << "==================================\n";
}

int main() {
    cout << fixed << setprecision(2);

    Bank bank;
    int choice;

    do {
        showMenu();
        choice = readInt("Enter your choice: ");
        cout << endl;

        switch (choice) {
            case 1:
                bank.createAccount();
                break;

            case 2:
                bank.depositMoney();
                break;

            case 3:
                bank.withdrawMoney();
                break;

            case 4:
                bank.checkAccount();
                break;

            case 5:
                bank.displayAllAccounts();
                break;

            case 6:
                bank.transferMoney();
                break;

            case 7:
                cout << "Thank you for using the Bank Management System. "
                     << "Goodbye!\n";
                break;

            default:
                cout << "Invalid choice! "
                     << "Please enter a number between 1 and 7.\n";
        }
    } while (choice != 7);

    return 0;
}
