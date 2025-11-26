#include "PageManager.h"
#include <fstream>
using namespace std;

PageManager::PageManager(const string& filename) : tableFile(filename) {
    loadPages();
}

int PageManager::appendRecord(const vector<string>& record) {
    string recordData;
    for (const auto& field : record) {
        recordData += to_string(field.length()) + "|" + field + "|";
    }
    
    if (pages.empty() || !pages.back()->hasSpace(recordData.length())) {
        pages.push_back(make_shared<Page>(pages.size()));
    }
    
    auto& page = pages.back();
    int offset = page->bytesUsed;
    page->bytesUsed += recordData.length();
    
    for (size_t i = 0; i < recordData.length(); i++) {
        page->data[offset + i] = recordData[i];
    }
    
    return (page->pageID << 16) | offset;
}

void PageManager::savePages() {
    ofstream file(tableFile, ios::binary);
    for (const auto& page : pages) {
        file.write(page->data.data(), PAGE_SIZE);
    }
}

void PageManager::loadPages() {
    ifstream file(tableFile, ios::binary);
    if (!file) return;
    
    vector<char> buffer(PAGE_SIZE);
    int pageID = 0;
    while (file.read(buffer.data(), PAGE_SIZE)) {
        auto page = make_shared<Page>(pageID++);
        page->data = buffer;
        pages.push_back(page);
    }
}
