#ifndef PAGEMANAGER_H
#define PAGEMANAGER_H

#include <string>
#include <vector>
#include <memory>
using namespace std;

const int PAGE_SIZE = 4096;  // 4KB pages like real databases

class Page {
public:
    int pageID;
    vector<char> data;
    int bytesUsed;
    
    Page(int id) : pageID(id), data(PAGE_SIZE, 0), bytesUsed(0) {}
    
    bool hasSpace(int recordSize) const { return bytesUsed + recordSize <= PAGE_SIZE; }
};

class PageManager {
private:
    vector<shared_ptr<Page>> pages;
    string tableFile;
    
public:
    PageManager(const string& filename);
    
    int appendRecord(const vector<string>& record);
    vector<string> readRecord(int pageID, int offset);
    void savePages();
    void loadPages();
};

#endif // PAGEMANAGER_H
