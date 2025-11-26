#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
using namespace std;

class Utils {
public:
    // String utilities
    static string trim(const string& str);
    static string toLower(const string& str);
    static vector<string> split(const string& str, char delimiter);
    static string join(const vector<string>& strings, const string& delimiter);

    // Validation
    static bool isValidIdentifier(const string& name);
    static bool isValidNumber(const string& str);
};

#endif // UTILS_H
