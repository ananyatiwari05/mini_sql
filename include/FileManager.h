#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>
using namespace std;    
class FileManager {
public:
    // File operations
    static bool fileExists(const string& filename);
    static bool createDirectory(const string& dirname);
    static vector<string> listFilesInDirectory(const string& dirname);

    // Read/Write operations
    static string readFile(const string& filename);
    static bool writeFile(const string& filename, const string& content);
    static vector<string> readLines(const string& filename);
    static bool writeLines(const string& filename, const vector<string>& lines);
};

#endif // FILE_MANAGER_H
