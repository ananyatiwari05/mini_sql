#include "Database.h"
#include "Utils.h"
#include "Colors.h"
#include <iostream>
#include <string>
using namespace std;

void clearScreen()
{
    cout << "\033[2J\033[H";
}

void displayHeader()
{
    cout << "\n";
    cout << Colors::BRIGHT_CYAN << "===========================" << Colors::RESET << "\n";
    cout << Colors::BRIGHT_CYAN << "  " << Colors::RESET
         << Colors::BOLD << Colors::BRIGHT_YELLOW << "MiniSQL Database Engine" << Colors::RESET
         << Colors::RESET << "\n";
    cout << Colors::BRIGHT_CYAN << "===========================" << Colors::RESET << "\n\n";
}

void displayWelcome()
{
    displayHeader();

    cout << Colors::BOLD << Colors::BRIGHT_MAGENTA << "Database Operations:" << Colors::RESET << "\n";
    cout << "  " << Colors::BRIGHT_GREEN << "CREATE DATABASE" << Colors::RESET << " mydb;\n";
    cout << "  " << Colors::BRIGHT_GREEN << "USE" << Colors::RESET << " mydb;\n";
    cout << "  " << Colors::BRIGHT_GREEN << "DROP TABLE" << Colors::RESET << " tablename;\n\n";

    cout << Colors::BOLD << Colors::BRIGHT_MAGENTA << "Table Operations:" << Colors::RESET << "\n";
    cout << "  " << Colors::BRIGHT_GREEN << "CREATE TABLE" << Colors::RESET << " users (id INT, name TEXT, age INT);\n";
    cout << "  " << Colors::BRIGHT_GREEN << "INSERT INTO" << Colors::RESET << " users VALUES (1, 'Alice', 25);\n";
    cout << "  " << Colors::BRIGHT_GREEN << "UPDATE" << Colors::RESET << " users SET age = 26 WHERE id = 1;\n";
    cout << "  " << Colors::BRIGHT_GREEN << "ALTER TABLE" << Colors::RESET << " users ADD email TEXT;\n";
    cout << "  " << Colors::BRIGHT_GREEN << "ALTER TABLE" << Colors::RESET << " users DROP email;\n\n";

    cout << Colors::BOLD << Colors::BRIGHT_MAGENTA << "Query Operations:" << Colors::RESET << "\n";
    cout << "  " << Colors::BRIGHT_GREEN << "SELECT" << Colors::RESET << " * FROM users;\n";
    cout << "  " << Colors::BRIGHT_GREEN << "SELECT" << Colors::RESET << " * FROM users WHERE age > 25;\n";
    cout << "  " << Colors::BRIGHT_GREEN << "SELECT" << Colors::RESET << " * FROM users ORDER BY age DESC;\n";
    cout << "  " << Colors::BRIGHT_GREEN << "SELECT" << Colors::RESET << " * FROM users GROUP BY age;\n";
    cout << "  " << Colors::BRIGHT_GREEN << "DELETE FROM" << Colors::RESET << " users WHERE id = 1;\n\n";

    cout << Colors::BOLD << Colors::BRIGHT_MAGENTA << "Shell Commands:" << Colors::RESET << "\n";
    cout << "  " << Colors::BRIGHT_YELLOW << "HELP" << Colors::RESET << "   - Show this guide\n";
    cout << "  " << Colors::BRIGHT_YELLOW << "CLEAR" << Colors::RESET << "  - Clear screen\n";
    cout << "  " << Colors::BRIGHT_YELLOW << "EXIT" << Colors::RESET << "   - Exit program\n";
    cout << "  " << Colors::BRIGHT_YELLOW << "CANCEL" << Colors::RESET << " - Cancel current query\n";
    cout << "  " << Colors::dim("(Remember to end all queries with semicolon ;)") << "\n\n";

    cout << Colors::BRIGHT_MAGENTA << "Current Database: " << Colors::RESET << Colors::BRIGHT_CYAN << "Not selected" << Colors::RESET << "\n\n";
}

string readStatement()
{
    string statement;
    string line;

    while (true)
    {
        if (statement.empty())
        {
            cout << Colors::BRIGHT_CYAN << "sql" << Colors::RESET
                 << Colors::BRIGHT_MAGENTA << ">" << Colors::RESET << " ";
        }
        else
        {
            cout << Colors::dim("   ...") << Colors::RESET << " ";
        }

        if (!getline(cin, line))
        {
            return "EXIT";
        }

        line = Utils::trim(line);

        if (Utils::toLower(line) == "cancel")
        {
            cout << Colors::warning("Query cancelled.") << "\n\n";
            return "";
        }

        if (line.empty())
            continue;

        statement += line + " ";

        // Check if statement ends with semicolon
        if (statement.find(';') != string::npos)
        {
            break;
        }
    }

    return statement;
}

int main()
{
    clearScreen();
    displayWelcome();

    Database db("databases");

    string input;
    while (true)
    {
        string dbName = db.getCurrentDatabase().empty() ? "none" : db.getCurrentDatabase();
        cout << Colors::BRIGHT_CYAN << "db(" << dbName << ")" << Colors::RESET
             << Colors::BRIGHT_MAGENTA << ">" << Colors::RESET << " ";

        input = readStatement();

        if (input.empty())
        {
            continue;
        }

        if (input == "EXIT")
        {
            break;
        }

        if (!input.empty() && input.back() == ';')
        {
            input.pop_back();
        }

        string lowerInput = Utils::toLower(input);

        if (lowerInput.find("clear") != string::npos)
        {
            clearScreen();
            displayHeader();
            continue;
        }

        if (lowerInput.find("exit") != string::npos)
        {
            cout << "\n"
                 << Colors::BRIGHT_CYAN << "============================================================================" << Colors::RESET << "\n";
            cout << Colors::BRIGHT_YELLOW << "Thank you for using MiniSQL!" << Colors::RESET << "\n";
            cout << Colors::BRIGHT_CYAN << "============================================================================" << Colors::RESET << "\n\n";
            break;
        }

        if (lowerInput.find("help") != string::npos)
        {
            clearScreen();
            displayWelcome();
            continue;
        }

        cout << "\n";
        string result = db.executeQuery(input);
        cout << result << "\n\n";
    }

    return 0;
}
