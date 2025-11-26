#ifndef RECORD_H
#define RECORD_H

#include <string>
#include <vector>
#include "Utils.h"
using namespace std;
// Represents a single row/record in a table
class Record {
private:
    vector<string> values;
public:
    Record();
    Record(const vector<string>& vals);

    // Getters and setters
    void addValue(const string& value);
    const string& getValue(size_t index) const;
    string& getValue(size_t index);
    size_t getSize() const;

    // Convert to/from CSV format
    string toCSV() const;
    static Record fromCSV(const string& line);
};

#endif // RECORD_H
