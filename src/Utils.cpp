#include "Utils.h"
#include <algorithm>
#include <cctype>
#include <sstream>
using namespace std;
string Utils::trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

string Utils::toLower(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(),
                  [](unsigned char c) { return tolower(c); });
    return result;
}

vector<string> Utils::split(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delimiter)) {
        token = trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

string Utils::join(const vector<string>& strings, const string& delimiter) {
    string result;
    for (size_t i = 0; i < strings.size(); ++i) {
        result += strings[i];
        if (i < strings.size() - 1) {
            result += delimiter;
        }
    }
    return result;
}

bool Utils::isValidIdentifier(const string& name) {
    if (name.empty()) return false;
    if (!isalpha(name[0]) && name[0] != '_') return false;
    for (char c : name) {
        if (!isalnum(c) && c != '_') return false;
    }
    return true;
}

bool Utils::isValidNumber(const string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!isdigit(c)) return false;
    }
    return true;
}
