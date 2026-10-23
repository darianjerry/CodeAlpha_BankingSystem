#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <limits>

using namespace std;

// Gets the current computer date and formats it neatly as DD/MM/YYYY
string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    string day = to_string(ltm->tm_mday);
    string month = to_string(1 + ltm->tm_mon); // Months start at 0, so add 1
    string year = to_string(1900 + ltm->tm_year); // Years start at 1900, so add 1900
    return day + "/" + month + "/" + year;
}

// Ensures the user enters a valid whole number within a specific range
int getValidInt(string prompt, int min, int max) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            if (value >= min && value <= max) {
                return value;
            }
            cout << "Invalid range! Must be between " << min << " and " << max << ".\n";
        } else {
            cout << "Invalid input! Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// Ensures the user enters a valid decimal amount without crashing the program
double getValidDouble(string prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            return value;
        } else {
            cout << "Invalid input! Please enter a valid amount.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// Transaction class to store and display individual financial movements
class Transaction {
public:
    string date;
    string type;
    string description;
    double amount;
    double balanceAfter;

    // Constructor to quickly initialize a transaction record
    Transaction(string d, string t, string desc, double amt, double bal) {
        date = d;
        type = t;
        description = desc;
        amount = amt;
        balanceAfter = bal;
    }

    // Prints out a single transaction line neatly
    void display() const {
        cout << "Date: " << setw(10) << date
             << " | Type: " << setw(12) << type
             << " | Desc: " << setw(15) << description
             << " | Amount: GHS " << setw(8) << fixed << setprecision(2) << amount
             << " | Bal After: GHS " << setw(8) << balanceAfter << endl;
    }
};

// Structure to hold individual expense details
struct Expense {
    string date;
    string description;
    double amount;
    int categoryIndex;
};

// Structure to track daily income entries
struct Income {
    string date;
    double amount;
};

// Account class to handle all money operations, savings, loans, and logs
class Account {
private:
    double initialBalance;
    double currentSavings;
    double totalBorrowed;
    vector<Transaction> transactionHistory;
    vector<Expense> expenses;
    vector<Income> incomes;
    vector<string> categories;

public:
    // Constructor sets up initial balances and default expense categories
    Account(double initial) {
        initialBalance = initial;
        currentSavings = 0.0;
        totalBorrowed = 0.0;
        categories = {"Food & Drinks", "Transportation", "Personal & Lifestyle", "Bills & Utilities", "Other"};
    }

    // Allows users to deposit money into their savings pool
    void saveMoney() {
        double accumulated = 0.0;
        char choice;
        do {
            double amt = getValidDouble("Enter the amount you want to save: ");
            if (amt > 0) {
                accumulated += amt;
            } else {
                cout << "Amount must be greater than zero.\n";
            }
            cout << "Do you want to save more? (y/n): ";
            cin >> choice;
        } while (choice == 'y' || choice == 'Y');

        currentSavings += accumulated;
        double netBal = initialBalance + currentSavings + totalBorrowed;
        transactionHistory.push_back(Transaction(getCurrentDate(), "Savings Deposit", "Added to Savings", accumulated, netBal));
        cout << "Successfully saved GHS " << fixed << setprecision(2) << accumulated << "! Total Savings: GHS " << currentSavings << endl;
    }

    // Lets users borrow a loan only if their initial balance is below 1.00 GHS
    void borrowMoney() {
        if (initialBalance >= 1.00) {
            cout << "You do not qualify for a loan because you have GHS " << fixed << setprecision(2) << initialBalance << " in your initial balance.\n";
            return;
        }
        double amt = getValidDouble("Enter the amount you want to borrow: ");
        if (amt > 0) {
            totalBorrowed += amt;
            double netBal = initialBalance + currentSavings + totalBorrowed;
            transactionHistory.push_back(Transaction(getCurrentDate(), "Loan", "Loan Borrowed", amt, netBal));
            cout << "Loan successful! Borrowed GHS " << fixed << setprecision(2) << amt << endl;
        } else {
            cout << "Invalid loan amount.\n";
        }
    }

    // Records a new expense item under a chosen category
    void addExpense() {
        Expense exp;
        exp.date = getCurrentDate();

        cout << "\nSelect Category:\n";
        for (size_t i = 0; i < categories.size(); i++) {
            cout << i + 1 << ". " << categories[i] << endl;
        }
        exp.categoryIndex = getValidInt("Choose category (1-5): ", 1, 5) - 1;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter expense description: ";
        getline(cin, exp.description);

        exp.amount = getValidDouble("Enter amount (GHS): ");
        expenses.push_back(exp);

        double netBal = initialBalance + currentSavings + totalBorrowed;
        transactionHistory.push_back(Transaction(exp.date, "Expense", exp.description, exp.amount, netBal));
        cout << "Expense recorded successfully!\n";
    }

    // Automatically checks and adds today's daily income wage if not already logged
    void addDailyIncome() {
        string today = getCurrentDate();
        for (const auto& inc : incomes) {
            if (inc.date == today) {
                return; // Already logged today
            }
        }
        Income inc;
        inc.date = today;
        cout << "\n[ FIRST ENTRY FOR TODAY: " << today << " ]\n";
        inc.amount = getValidDouble("Enter your daily income/wage for today (GHS): ");
        incomes.push_back(inc);
        cout << "Daily income recorded successfully!\n";
    }

    // Displays the most recent transaction history logs
    void viewRecentTransactions() const {
        if (transactionHistory.empty()) {
            cout << "\nNo transactions recorded yet.\n";
            return;
        }
        cout << "\n============================== RECENT TRANSACTIONS ==============================\n";
        int start = max(0, (int)transactionHistory.size() - 10);
        for (size_t i = start; i < transactionHistory.size(); i++) {
            cout << i + 1 << ". ";
            transactionHistory[i].display();
        }
        cout << "=================================================================================\n";
    }

    // Displays overall financial account metrics
    void checkBalance() const {
        double totalExpenseAmount = 0.0;
        for (const auto& exp : expenses) totalExpenseAmount += exp.amount;
        double totalIncomeAmount = 0.0;
        for (const auto& inc : incomes) totalIncomeAmount += inc.amount;

        cout << "\n----------------------------------------\n";
        cout << " 1. Initial Balance:        GHS " << fixed << setprecision(2) << initialBalance << "\n";
        cout << " 2. Total Amount Saved:     GHS " << currentSavings << "\n";
        cout << " 3. Total Amount Borrowed:  GHS " << totalBorrowed << "\n";
        cout << " 4. Total Income Recorded:  GHS " << totalIncomeAmount << "\n";
        cout << " 5. Total Expenses Spent:   GHS " << totalExpenseAmount << "\n";
        cout << "----------------------------------------\n";
    }

    // Shows full dashboard overview combining balance and transactions
    void showOverviewDashboard() const {
        checkBalance();
        viewRecentTransactions();
    }
};

// Customer class to manage profile details, credentials, and user interaction
class Customer {
private:
    string name;
    string username;
    string pin;
    Account account;

public:
    // Customer constructor initializes profile and links an Account object
    Customer(string n, string u, string p, double initialBal) : account(initialBal) {
        name = n;
        username = u;
        pin = p;
    }

    // Verifies if login credentials match
    bool authenticate(string u, string p) const {
        return (username == u && pin == p);
    }

    string getName() const { return name; }

    // Main interactive dashboard loop for the logged-in customer
    void handleDashboard() {
        int choice;
        do {
            cout << "\n========== BANKING SYSTEM DASHBOARD: Welcome, " << name << " ==========\n";
            cout << "1. Save Money\n";
            cout << "2. Borrow Money (Loans)\n";
            cout << "3. Add Expense\n";
            cout << "4. Record Daily Income\n";
            cout << "5. Check Balance Details\n";
            cout << "6. View Recent Transactions\n";
            cout << "7. View Overall Financial Summary Dashboard\n";
            cout << "0. Logout\n";
            choice = getValidInt("Choose an option: ", 0, 7);

            switch (choice) {
                case 1:
                    account.saveMoney();
                    break;
                case 2:
                    account.borrowMoney();
                    break;
                case 3:
                    account.addExpense();
                    break;
                case 4:
                    account.addDailyIncome();
                    break;
                case 5:
                    account.checkBalance();
                    break;
                case 6:
                    account.viewRecentTransactions();
                    break;
                case 7:
                    account.showOverviewDashboard();
                    break;
                case 0:
                    cout << "Logging out...\n";
                    break;
            }
        } while (choice != 0);
    }
};

// Main entry point of the application
int main() {
    // Create a demo customer account for testing (Username: wesley, PIN: 1234)
    string name, inputUser, inputPin;

    cout << "========================================\n";
    cout << "   WELCOME TO THE BANK  \n";
    cout << "========================================\n";
    cout << "--- Create Your Account ---\n";
    cout << "Enter your full name: ";
    cin.ignore();
    getline(cin, name);
    cout << "Choose a username: ";
    cin >> inputUser;
    cout << "Choose a 4-digit PIN/Password: ";
    cin >> inputPin;

    // Create the customer object with the user's custom details (starting with 0.00 initial balance)
    Customer customer(name, inputUser, inputPin, 0.00);

    // Clear the screen or print a separator for the login phase
    cout << "\nAccount created successfully! Please login to continue.\n";
    cout << "----------------------------------------\n";

    string loginUser, loginPin;
    cout << "Enter username: ";
    cin >> loginUser;
    cout << "Enter PIN/Password: ";
    cin >> loginPin;

    // Authenticate using the custom credentials provided
    if (customer.authenticate(loginUser, loginPin)) {
        cout << "\nLogin Successful! Welcome back, " << customer.getName() << ".\n";
        customer.handleDashboard();
    } else {
        cout << "\nAuthentication Failed! Incorrect username or PIN.\n";
    }

    cout << "\nThank you for using my system. Goodbye!\n";
    return 0;
}

