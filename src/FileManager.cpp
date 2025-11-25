#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>

bool FileManager::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

bool FileManager::createDirectory(const std::string& dirname) {
    #ifdef _WIN32
        return mkdir(dirname.c_str()) == 0 || errno == EEXIST;
    #else
        return mkdir(dirname.c_str(), 0755) == 0 || errno == EEXIST;
    #endif
}

std::vector<std::string> FileManager::listFilesInDirectory(const std::string& dirname) {
    std::vector<std::string> files;
    DIR* dir = opendir(dirname.c_str());
    if (!dir) return files;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        if (filename.find(".tbl") != std::string::npos) {
            files.push_back(dirname + "/" + filename);
        }
    }
    closedir(dir);
    return files;
}

std::string FileManager::readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return "";

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool FileManager::writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    file << content;
    return file.good();
}

std::vector<std::string> FileManager::readLines(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream file(filename);
    if (!file.is_open()) return lines;

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

bool FileManager::writeLines(const std::string& filename, const std::vector<std::string>& lines) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    for (const auto& line : lines) {
        file << line << "\n";
    }
    return file.good();
}
