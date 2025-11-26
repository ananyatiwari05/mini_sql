#include "Database.h"
#include "Utils.h"
#include "Colors.h"
#include <iostream>
#include <string>
using namespace std;
void clearScreen() {
    cout << "\033[2J\033[H";
}

void displayHeader() {
    cout << "\n";
    cout << Colors::BRIGHT_CYAN << "+-----+-----+-----+-----+-----+-----+-----+-----+" << Colors::RESET << "\n";
    cout << Colors::BRIGHT_CYAN << "|" << Colors::RESET 
              << Colors::BOLD << Colors::BRIGHT_YELLOW << "         MiniSQL Database         " << Colors::RESET 
              << Colors::BRIGHT_CYAN << "       |" << Colors::RESET << "\n";
    cout << Colors::BRIGHT_CYAN << "+-----+-----+-----+-----+-----+-----+-----+-----+" << Colors::RESET << "\n\n";
}

void displayWelcome() {
    displayHeader();
    
    cout << Colors::BOLD << Colors::BRIGHT_MAGENTA << "Quick Start Guide:" << Colors::RESET << "\n";
    cout << "  " << Colors::BRIGHT_GREEN << "CREATE TABLE" << Colors::RESET << " users (id INTEGER, name TEXT, age INTEGER);\n";
    cout << "  " << Colors::BRIGHT_GREEN << "INSERT INTO" << Colors::RESET << " users VALUES (1, 'Alice', 25);\n";
    cout << "  " << Colors::BRIGHT_GREEN << "SELECT" << Colors::RESET << " * FROM users;\n";
    cout << "  " << Colors::BRIGHT_GREEN << "SELECT" << Colors::RESET << " * FROM users WHERE age > 20;\n";
    cout << "  " << Colors::BRIGHT_GREEN << "DELETE FROM" << Colors::RESET << " users WHERE id = 1;\n\n";

    cout << Colors::BOLD << Colors::BRIGHT_MAGENTA << "Commands:" << Colors::RESET << "\n";
    cout << "  " << Colors::BRIGHT_YELLOW << "HELP" << Colors::RESET << "   - Show this guide\n";
    cout << "  " << Colors::BRIGHT_YELLOW << "CLEAR" << Colors::RESET << "  - Clear screen\n";
    cout << "  " << Colors::BRIGHT_YELLOW << "EXIT" << Colors::RESET << "   - Exit program\n";
    cout << "  " << Colors::BRIGHT_YELLOW << "CANCEL" << Colors::RESET << " - Cancel current query\n\n";
}

string readStatement() {
    string statement;
    string line;
    int lineCount = 0;

    while (true) {
        if (statement.empty()) {
            cout << Colors::BRIGHT_CYAN << "sql" << Colors::RESET 
                      << Colors::BRIGHT_MAGENTA << ">" << Colors::RESET << " ";
        } else {
            cout << Colors::dim("  ...") << " ";
        }

        if (!getline(cin, line)) {
            return "EXIT";  // Handle EOF
        }

        line = Utils::trim(line);

        if (Utils::toLower(line) == "cancel") {
            cout << Colors::warning("Query cancelled.") << "\n\n";
            return "";
        }

        if (line.empty()) continue;

        statement += line + " ";
        lineCount++;

        // Check if statement ends with semicolon - only then execute
        if (statement.find(';') != string::npos) {
            break;
        }
    }

    return statement;
}

void displayDivider() {
    cout << Colors::dim("--------------------------------------------------------------------------\n");
}

void displaySuccess(const string& message) {
    cout << Colors::BRIGHT_GREEN << "[OK]" << Colors::RESET << " " << message << "\n";
}

int main() {
    clearScreen();
    displayWelcome();

    // Initialize database
    Database db("data");

    string input;
    while (true) {
        input = readStatement();

        // Handle empty input or EOF
        if (input.empty()) {
            continue;
        }

        // Handle EXIT from readStatement
        if (input == "EXIT") {
            break;
        }

        // Remove trailing semicolon
        if (!input.empty() && input.back() == ';') {
            input.pop_back();
        }

        string lowerInput = Utils::toLower(input);

        // Check for clear command
        if (lowerInput.find("clear") != string::npos) {
            clearScreen();
            displayHeader();
            continue;
        }

        // Check for exit command
        if (lowerInput.find("exit") != string::npos) {
            displayDivider();
            cout << Colors::BRIGHT_CYAN << "Goodbye!" << Colors::RESET << "\n\n";
            break;
        }

        // Check for help command
        if (lowerInput.find("help") != string::npos) {
            clearScreen();
            displayWelcome();
            continue;
        }

        // Execute the query
        cout << "\n";
        string result = db.executeQuery(input);
        cout << result << "\n";
    }

    return 0;
}
