#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
using namespace std;

bool FileManager::fileExists(const string& filename) {
    ifstream file(filename);
    return file.good();
}

bool FileManager::createDirectory(const string& dirname) {
    #ifdef _WIN32
        return mkdir(dirname.c_str()) == 0 || errno == EEXIST;
    #else
        return mkdir(dirname.c_str(), 0755) == 0 || errno == EEXIST;
    #endif
}

vector<string> FileManager::listFilesInDirectory(const string& dirname) {
    vector<string> files;
    DIR* dir = opendir(dirname.c_str());
    if (!dir) return files;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        string filename = entry->d_name;
        if (filename.find(".tbl") != string::npos) {
            files.push_back(dirname + "/" + filename);
        }
    }
    closedir(dir);
    return files;
}

string FileManager::readFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return "";

    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool FileManager::writeFile(const string& filename, const string& content) {
    ofstream file(filename);
    if (!file.is_open()) return false;

    file << content;
    return file.good();
}

vector<string> FileManager::readLines(const string& filename) {
    vector<string> lines;
    ifstream file(filename);
    if (!file.is_open()) return lines;

    string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

bool FileManager::writeLines(const string& filename, const vector<string>& lines) {
    ofstream file(filename);
    if (!file.is_open()) return false;

    for (const auto& line : lines) {
        file << line << "\n";
    }
    return file.good();
}
