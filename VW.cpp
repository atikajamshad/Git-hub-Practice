#include <iostream>		 // For console input/output
#include <fstream>		 // For file input/output
#include <string>		 // For string operations
#include <unordered_map> // For unordered maps
#include <sstream>		 // For string streams
#include <iomanip>		 // For output formatting
#include <ctime>		 // For date and time functions
#include <vector>		 // For vectors
#include <filesystem>	 // For file system operations
#include <regex>		 // For regular expressions
#include <limits>		 // For numeric_limits

using namespace std;
namespace fs = std::filesystem;

// Low balance alert threshold
const int LOW_BALANCE_THRESHOLD = 100;

// Utility to hash passwords using a simple custom algorithm
string hashPassword(const string &password)
{
	string hashed = "";
	for (char c : password)
	{
		hashed += to_string((c * 7) % 256); // Simple transformation
	}
	return hashed;
}

// Function to register a new user
void registerUser()
{
	string username, password;
	cout << " Enter a username: ";
	cin >> username;
	cout << " Enter a password: ";
	cin >> password;

	string hashedPassword = hashPassword(password);

	// Save user credentials to file
	ofstream userFile("users.txt", ios::app);
	if (userFile.is_open())
	{
		userFile << username << " " << hashedPassword << endl;
		userFile.close();

		// Initialize user balance
		ofstream balanceFile("balances.txt", ios::app);
		if (balanceFile.is_open())
		{
			balanceFile << username << " 0" << endl;
			balanceFile.close();
		}

		cout << "--> Registration successful!" << endl;
	}
	else
	{
		cerr << "--> Error: Unable to save user data." << endl;
	}
}

// Function to log in an existing user
bool loginUser()
{
	string username, password;
	cout << " Enter your username: ";
	cin >> username;
	cout << " Enter your password: ";
	cin >> password;

	string hashedPassword = hashPassword(password);

	// Read user credentials from file
	ifstream userFile("users.txt");
	string storedUsername, storedPassword;

	if (userFile.is_open())
	{
		while (userFile >> storedUsername >> storedPassword)
		{
			if (storedUsername == username && storedPassword == hashedPassword)
			{
				cout << "--> Login successful!" << endl;
				userFile.close();
				return true;
			}
		}
		userFile.close();
		cout << "--> Invalid username or password." << endl;
	}
	else
	{
		cerr << "--> Error: Unable to read user data." << endl;
	}
	return false;
}

// Function to reset a user's password
void resetPassword()
{
	string username, newPassword;
	cout << " Enter your username: ";
	cin >> username;
	cout << " Enter your new password: ";
	cin >> newPassword;

	string hashedPassword = hashPassword(newPassword);

	ifstream userFile("users.txt");
	ofstream tempFile("temp.txt");
	string storedUsername, storedPassword;
	bool userFound = false;

	if (userFile.is_open() && tempFile.is_open())
	{
		while (userFile >> storedUsername >> storedPassword)
		{
			if (storedUsername == username)
			{
				tempFile << username << " " << hashedPassword << endl;
				userFound = true;
			}
			else
			{
				tempFile << storedUsername << " " << storedPassword << endl;
			}
		}
		userFile.close();
		tempFile.close();

		// Replace the original file with the updated file
		remove("users.txt");
		rename("temp.txt", "users.txt");

		if (userFound)
		{
			cout << "--> Password reset successful!" << endl;
		}
		else
		{
			cout << "--> Username not found." << endl;
		}
	}
	else
	{
		cerr << "--> Error: Unable to process request." << endl;
	}
}

// Function to get the balance of a user
int getBalance(const string &username)
{
	ifstream balanceFile("balances.txt");
	string storedUsername;
	int balance;

	if (balanceFile.is_open())
	{
		while (balanceFile >> storedUsername >> balance)
		{
			if (storedUsername == username)
			{
				balanceFile.close();
				return balance;
			}
		}
		balanceFile.close();
	}
	return -1; // Indicates balance not found
}

// Function to update the balance of a user
void updateBalance(const string &username, int newBalance)
{
	ifstream balanceFile("balances.txt");
	ofstream tempFile("temp_balances.txt");
	string storedUsername;
	int balance;

	if (balanceFile.is_open() && tempFile.is_open())
	{
		while (balanceFile >> storedUsername >> balance)
		{
			if (storedUsername == username)
			{
				tempFile << username << " " << newBalance << endl;
			}
			else
			{
				tempFile << storedUsername << " " << balance << endl;
			}
		}
		balanceFile.close();
		tempFile.close();

		remove("balances.txt");
		rename("temp_balances.txt", "balances.txt");
	}
	else
	{
		cerr << "--> Error: Unable to update balance." << endl;
	}
}

// Function to view wallet balance
void viewBalance(const string &username)
{
	int balance = getBalance(username);
	if (balance != -1)
	{
		cout << "--> Your current balance is: " << balance << endl;
	}
	else
	{
		cout << "--> Balance not found." << endl;
	}
}

// Function to deposit funds with exception handling
void depositFunds(const string &username)
{
	try
	{
		int amount;
		cout << " Enter the amount to deposit: ";
		cin >> amount;

		if (cin.fail())
		{
			throw invalid_argument("Invalid input. Please enter a numeric value.");
		}

		if (amount <= 0)
		{
			throw invalid_argument("Deposit amount must be greater than zero.");
		}

		int currentBalance = getBalance(username);
		if (currentBalance == -1)
		{
			throw runtime_error("Unable to retrieve the current balance.");
		}

		updateBalance(username, currentBalance + amount);
		cout << "--> Deposit successful! Your new balance is: " << currentBalance + amount << endl;
	}
	catch (const invalid_argument &e)
	{
		cerr << "--> Error: " << e.what() << endl;
	}
	catch (const runtime_error &e)
	{
		cerr << "--> Error: " << e.what() << endl;
	}
	catch (...)
	{
		cerr << "--> An unexpected error occurred during deposit." << endl;
	}
}

// Function to withdraw funds with exception handling
void withdrawFunds(const string &username)
{
	try
	{
		int amount;
		cout << " Enter the amount to withdraw: ";
		cin >> amount;

		if (cin.fail())
		{
			throw invalid_argument("Invalid input. Please enter a numeric value.");
		}

		if (amount <= 0)
		{
			throw invalid_argument("Withdrawal amount must be greater than zero.");
		}

		int currentBalance = getBalance(username);
		if (currentBalance == -1)
		{
			throw runtime_error("Unable to retrieve the current balance.");
		}

		if (currentBalance < amount)
		{
			throw runtime_error("Insufficient balance to complete the withdrawal.");
		}

		updateBalance(username, currentBalance - amount);
		cout << "--> Withdrawal successful! Your new balance is: " << currentBalance - amount << endl;
	}
	catch (const invalid_argument &e)
	{
		cerr << "--> Error: " << e.what() << endl;
	}
	catch (const runtime_error &e)
	{
		cerr << "--> Error: " << e.what() << endl;
	}
	catch (...)
	{
		cerr << "--> An unexpected error occurred during withdrawal." << endl;
	}
}

// Utility to get the current timestamp as a string
string getCurrentTimestamp()
{
	time_t now = time(0);
	tm *ltm = localtime(&now);
	stringstream ss;
	ss << 1900 + ltm->tm_year << "-"
	   << setw(2) << setfill('0') << 1 + ltm->tm_mon << "-"
	   << setw(2) << setfill('0') << ltm->tm_mday << " "
	   << setw(2) << setfill('0') << ltm->tm_hour << ":"
	   << setw(2) << setfill('0') << ltm->tm_min << ":"
	   << setw(2) << setfill('0') << ltm->tm_sec;
	return ss.str();
}

// Function to log a transaction
void logTransaction(const string &username, const string &type, int amount, const string &description = "", const string &category = "")
{
	ofstream transactionFile("transactions.txt", ios::app);
	if (transactionFile.is_open())
	{
		transactionFile << username << "|" << type << "|" << amount << "|" << getCurrentTimestamp() << "|" << description << "|" << category << endl;
		transactionFile.close();
	}
	else
	{
		cerr << "--> Error: Unable to log transaction." << endl;
	}
}

// Function to view transaction history
void viewTransactionHistory(const string &username)
{
	ifstream transactionFile("transactions.txt");
	string line, storedUsername, type, timestamp, description, category;
	int amount;

	if (transactionFile.is_open())
	{
		cout << "\n--- Transaction History for " << username << " ---\n";
		cout << left << setw(10) << "Type" << setw(10) << "Amount" << setw(20) << "Timestamp" << setw(15) << "Category" << "Description" << endl;
		while (getline(transactionFile, line))
		{
			stringstream ss(line);
			getline(ss, storedUsername, '|');
			getline(ss, type, '|');
			ss >> amount;
			ss.ignore();
			getline(ss, timestamp, '|');
			getline(ss, description, '|');
			getline(ss, category, '|');

			if (storedUsername == username)
			{
				cout << left << setw(10) << type << setw(10) << amount << setw(20) << timestamp << setw(15) << category << description << endl;
			}
		}
		transactionFile.close();
	}
	else
	{
		cerr << "--> Error: Unable to read transaction history." << endl;
	}
}

// Function to view spending summary by category
void viewSpendingSummary(const string &username)
{
	ifstream transactionFile("transactions.txt");
	string line, storedUsername, type, category;
	int amount;
	unordered_map<string, int> categoryTotals;

	if (transactionFile.is_open())
	{
		while (getline(transactionFile, line))
		{
			stringstream ss(line);
			getline(ss, storedUsername, '|');
			getline(ss, type, '|');
			ss >> amount;
			ss.ignore(numeric_limits<streamsize>::max(), '|'); // Skip timestamp
			ss.ignore(numeric_limits<streamsize>::max(), '|'); // Skip description
			getline(ss, category, '|');

			if (storedUsername == username && type == "Withdrawal")
			{
				categoryTotals[category] += amount;
			}
		}
		transactionFile.close();

		cout << "\n--- Spending Summary for " << username << " ---\n";
		for (const auto &entry : categoryTotals)
		{
			cout << left << setw(15) << entry.first << entry.second << endl;
		}
	}
	else
	{
		cerr << "--> Error: Unable to read transactions for spending summary." << endl;
	}
}

// Function to generate account summary
void generateAccountSummary(const string &username)
{
	int balance = getBalance(username);
	cout << "\n--- Account Summary for " << username << " ---\n";
	cout << "Current Balance: " << balance << endl;
	viewSpendingSummary(username);
	viewTransactionHistory(username);
}

// Function to check and alert low balance
void checkLowBalance(const string &username)
{
	int balance = getBalance(username);
	if (balance < LOW_BALANCE_THRESHOLD)
	{
		cout << "\n--> Alert: Your balance is below " << LOW_BALANCE_THRESHOLD << " units. Current balance: " << balance << endl;
	}
}

// Function to validate password strength
bool validatePassword(const string &password)
{
	if (password.length() < 8)
	{
		cout << "--> Password must be at least 8 characters long." << endl;
		return false;
	}
	if (!regex_search(password, regex("[A-Z]")))
	{
		cout << "--> Password must contain at least one uppercase letter." << endl;
		return false;
	}
	if (!regex_search(password, regex("[a-z]")))
	{
		cout << "--> Password must contain at least one lowercase letter." << endl;
		return false;
	}
	if (!regex_search(password, regex("[0-9]")))
	{
		cout << "--> Password must contain at least one number." << endl;
		return false;
	}
	if (!regex_search(password, regex("[!@#$%^&*]")))
	{
		cout << "--> Password must contain at least one special character (!@#$%^&*)." << endl;
		return false;
	}
	return true;
}

// Function to log admin actions
void logAdminAction(const string &action)
{
	ofstream logFile("admin_audit.log", ios::app);
	if (logFile.is_open())
	{
		logFile << getCurrentTimestamp() << " | " << action << endl;
		logFile.close();
	}
	else
	{
		cerr << "--> Error: Unable to log admin action." << endl;
	}
}

// Function to transfer funds between users
void transferFunds(const string &sender)
{
	string recipient;
	int amount;
	cout << " Enter the recipient's username: ";
	cin >> recipient;
	cout << " Enter the amount to transfer: ";
	cin >> amount;

	if (amount > 0)
	{
		int senderBalance = getBalance(sender);
		int recipientBalance = getBalance(recipient);

		if (senderBalance == -1)
		{
			cout << "--> Error: Unable to retrieve sender's balance." << endl;
			return;
		}
		if (recipientBalance == -1)
		{
			cout << "--> Error: Recipient does not exist." << endl;
			return;
		}

		if (senderBalance >= amount)
		{
			updateBalance(sender, senderBalance - amount);
			updateBalance(recipient, recipientBalance + amount);

			logTransaction(sender, "Transfer Out", amount, "To: " + recipient);
			logTransaction(recipient, "Transfer In", amount, "From: " + sender);

			cout << "--> Transfer successful! Your new balance is: " << senderBalance - amount << endl;
		}
		else
		{
			cout << "--> Insufficient balance." << endl;
		}
	}
	else
	{
		cout << "--> Invalid amount." << endl;
	}
}

// Admin: View all users and balances
void viewAllUsers()
{
	ifstream userFile("users.txt");
	ifstream balanceFile("balances.txt");
	string username, hashedPassword;
	string storedUsername;
	int balance;

	cout << "\n--- User List ---\n";
	cout << left << setw(20) << "Username" << "Balance" << endl;

	if (userFile.is_open() && balanceFile.is_open())
	{
		while (userFile >> username >> hashedPassword)
		{
			balanceFile.clear();
			balanceFile.seekg(0, ios::beg);
			while (balanceFile >> storedUsername >> balance)
			{
				if (username == storedUsername)
				{
					cout << left << setw(20) << username << balance << endl;
					break;
				}
			}
		}
		userFile.close();
		balanceFile.close();
	}
	else
	{
		cerr << "--> Error: Unable to read user or balance data." << endl;
	}
}

// Admin: View all transactions
void viewAllTransactions()
{
	ifstream transactionFile("transactions.txt");
	string line, username, type, timestamp, description, category;
	int amount;

	cout << "\n--- All Transactions ---\n";
	cout << left << setw(15) << "Username" << setw(10) << "Type" << setw(10) << "Amount" << setw(20) << "Timestamp" << setw(15) << "Category" << "Description" << endl;

	if (transactionFile.is_open())
	{
		while (getline(transactionFile, line))
		{
			stringstream ss(line);
			getline(ss, username, '|');
			getline(ss, type, '|');
			ss >> amount;
			ss.ignore();
			getline(ss, timestamp, '|');
			getline(ss, description, '|');
			getline(ss, category, '|');

			cout << left << setw(15) << username << setw(10) << type << setw(10) << amount << setw(20) << timestamp << setw(15) << category << description << endl;
		}
		transactionFile.close();
	}
	else
	{
		cerr << "--> Error: Unable to read transactions." << endl;
	}
}

// Admin: Reset a user's password with validation
void resetUserPassword()
{
	string username, newPassword;
	cout << " Enter the username to reset password: ";
	cin >> username;

	do
	{
		cout << " Enter the new password: ";
		cin >> newPassword;
	} while (!validatePassword(newPassword));

	ifstream userFile("users.txt");
	ofstream tempFile("temp_users.txt");
	string storedUsername, storedPassword;
	bool userFound = false;

	if (userFile.is_open() && tempFile.is_open())
	{
		while (userFile >> storedUsername >> storedPassword)
		{
			if (storedUsername == username)
			{
				tempFile << username << " " << hashPassword(newPassword) << endl;
				userFound = true;
			}
			else
			{
				tempFile << storedUsername << " " << storedPassword << endl;
			}
		}
		userFile.close();
		tempFile.close();

		remove("users.txt");
		rename("temp_users.txt", "users.txt");

		if (userFound)
		{
			cout << "--> Password reset successful for user: " << username << endl;
			logAdminAction("--> Password reset for user: " + username);
		}
		else
		{
			cout << "--> Username not found." << endl;
		}
	}
	else
	{
		cerr << "--> Error: Unable to reset user password." << endl;
	}
}

// Admin: Generate system-wide financial summary
void generateFinancialSummary()
{
	ifstream transactionFile("transactions.txt");
	string line, username, type, category;
	int amount;
	unordered_map<string, int> totals;

	if (transactionFile.is_open())
	{
		while (getline(transactionFile, line))
		{
			stringstream ss(line);
			getline(ss, username, '|');
			getline(ss, type, '|');
			ss >> amount;
			ss.ignore(numeric_limits<streamsize>::max(), '|');
			ss.ignore(numeric_limits<streamsize>::max(), '|');
			getline(ss, category, '|');

			totals[type] += amount;
		}
		transactionFile.close();

		cout << "\n--- Financial Summary ---\n";
		for (const auto &entry : totals)
		{
			cout << left << setw(15) << entry.first << entry.second << endl;
		}
	}
	else
	{
		cerr << "--> Error: Unable to read transactions for financial summary." << endl;
	}
}

// Function to back up files
void backupFiles()
{
	string timestamp = getCurrentTimestamp();
	string backupDir = "backups/";

	// Create backups directory if it doesn't exist
	if (!fs::exists(backupDir))
	{
		fs::create_directory(backupDir);
	}

	try
	{
		fs::copy_file("users.txt", backupDir + "users_" + timestamp + ".txt", fs::copy_options::overwrite_existing);
		fs::copy_file("balances.txt", backupDir + "balances_" + timestamp + ".txt", fs::copy_options::overwrite_existing);
		fs::copy_file("transactions.txt", backupDir + "transactions_" + timestamp + ".txt", fs::copy_options::overwrite_existing);

		cout << "Backup completed successfully! Files saved in the 'backups' directory." << endl;
		logAdminAction("Backup completed: " + timestamp);
	}
	catch (const fs::filesystem_error &e)
	{
		cerr << "--> Error during backup: " << e.what() << endl;
	}
}

// Function to restore files from a backup
void restoreFiles()
{
	string backupDir = "backups/";
	string usersBackup, balancesBackup, transactionsBackup;

	cout << " Available backups in the 'backups' directory:" << endl;
	for (const auto &entry : fs::directory_iterator(backupDir))
	{
		cout << entry.path().filename().string() << endl;
	}

	cout << "\nEnter the backup filenames you want to restore (e.g., users_2025-01-01_12-00-00.txt):" << endl;

	cout << "Users file: ";
	cin >> usersBackup;
	cout << "Balances file: ";
	cin >> balancesBackup;
	cout << "Transactions file: ";
	cin >> transactionsBackup;

	try
	{
		fs::copy_file(backupDir + usersBackup, "users.txt", fs::copy_options::overwrite_existing);
		fs::copy_file(backupDir + balancesBackup, "balances.txt", fs::copy_options::overwrite_existing);
		fs::copy_file(backupDir + transactionsBackup, "transactions.txt", fs::copy_options::overwrite_existing);

		cout << "--> Files restored successfully from backup!" << endl;
		logAdminAction("--> Files restored from backup: " + usersBackup + ", " + balancesBackup + ", " + transactionsBackup);
	}
	catch (const fs::filesystem_error &e)
	{
		cerr << "--> Error during restoration: " << e.what() << endl;
	}
}

// Utility function to clear input buffer
void clearInputBuffer()
{
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main()
{
	int choice = 0;
	string currentUsername;
	bool isAdmin = false;
	bool loggedIn = false;
	bool exitProgram = false;

	do
	{
		cout << "\n---***----- Virtual Wallet System -----***---\n";

		if (!loggedIn)
		{
			cout << " 1. Register\n"
				 << " 2. Login\n"
				 << " 3. Login as Admin\n"
				 << " 4. Exit\n";
			cout << "Choose an option: ";

			if (!(cin >> choice))
			{
				clearInputBuffer();
				cout << "--> Invalid input. Please enter a number.\n";
				continue;
			}

			switch (choice)
			{
			case 1:
				registerUser();
				break;

			case 2:
				if (loginUser())
				{
					cout << " Enter your username again to confirm login: ";
					clearInputBuffer();
					getline(cin, currentUsername);
					loggedIn = true;
				}
				break;

			case 3:
			{
				cout << " Enter admin password: ";
				string adminPassword;
				clearInputBuffer();
				getline(cin, adminPassword);

				if (adminPassword == "admin123")
				{
					isAdmin = true;
					loggedIn = true;
					cout << "--> Admin login successful.\n";
				}
				else
				{
					cout << "--> Invalid admin password.\n";
				}
				break;
			}

			case 4:
				exitProgram = true;
				cout << "--> Exiting the program... Goodbye!\n";
				break;

			default:
				cout << "--> Invalid choice. Please try again.\n";
			}
		}
		else
		{
			if (isAdmin)
			{
				cout << "\n---***----- Admin Operations -----***---\n"
					 << " 1. View All Users\n"
					 << " 2. View All Transactions\n"
					 << " 3. Reset User Password\n"
					 << " 4. Generate Financial Summary\n"
					 << " 5. Backup Files\n"
					 << " 6. Restore Files\n"
					 << " 7. Logout\n";
				cout << "Choose an option: ";

				if (!(cin >> choice))
				{
					clearInputBuffer();
					cout << "--> Invalid input. Please enter a number.\n";
					continue;
				}

				switch (choice)
				{
				case 1:
					viewAllUsers();
					break;
				case 2:
					viewAllTransactions();
					break;
				case 3:
					resetUserPassword();
					break;
				case 4:
					generateFinancialSummary();
					break;
				case 5:
					backupFiles();
					break;
				case 6:
					restoreFiles();
					break;
				case 7:
					cout << "--> Logging out...\n";
					loggedIn = false;
					isAdmin = false;
					break;
				default:
					cout << "--> Invalid choice. Please try again.\n";
				}
			}
			else
			{
				cout << "\n---***----- Wallet Operations -----***---\n"
					 << " 1. View Balance\n"
					 << " 2. Deposit Funds\n"
					 << " 3. Withdraw Funds\n"
					 << " 4. View Transaction History\n"
					 << " 5. Transfer Funds\n"
					 << " 6. View Spending Summary\n"
					 << " 7. Generate Account Summary\n"
					 << " 8. Logout\n";
				cout << "Choose an option: ";

				if (!(cin >> choice))
				{
					clearInputBuffer();
					cout << "--> Invalid input. Please enter a number.\n";
					continue;
				}

				switch (choice)
				{
				case 1:
					viewBalance(currentUsername);
					break;
				case 2:
					depositFunds(currentUsername);
					break;
				case 3:
					withdrawFunds(currentUsername);
					break;
				case 4:
					viewTransactionHistory(currentUsername);
					break;
				case 5:
					transferFunds(currentUsername);
					break;
				case 6:
					viewSpendingSummary(currentUsername);
					break;
				case 7:
					generateAccountSummary(currentUsername);
					break;
				case 8:
					cout << "--> Logging out...\n";
					loggedIn = false;
					break;
				default:
					cout << "--> Invalid choice. Please try again.\n";
				}
			}
		}
	} while (!exitProgram && (choice != 4 || loggedIn));

	return 0;
}