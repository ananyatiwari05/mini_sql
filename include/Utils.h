#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

class Utils {
public:
    // String utilities
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string join(const std::vector<std::string>& strings, const std::string& delimiter);

    // Validation
    static bool isValidIdentifier(const std::string& name);
    static bool isValidNumber(const std::string& str);
};

#endif // UTILS_H