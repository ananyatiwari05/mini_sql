#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>

class FileManager {
public:
    // File operations
    static bool fileExists(const std::string& filename);
    static bool createDirectory(const std::string& dirname);
    static std::vector<std::string> listFilesInDirectory(const std::string& dirname);

    // Read/Write operations
    static std::string readFile(const std::string& filename);
    static bool writeFile(const std::string& filename, const std::string& content);
    static std::vector<std::string> readLines(const std::string& filename);
    static bool writeLines(const std::string& filename, const std::vector<std::string>& lines);
};

#endif // FILE_MANAGER_H
