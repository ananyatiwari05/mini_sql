#ifndef RECORD_H
#define RECORD_H

#include <string>
#include <vector>
#include "Utils.h"

// Represents a single row/record in a table
class Record {
private:
    std::vector<std::string> values;

public:
    Record();
    Record(const std::vector<std::string>& vals);

    // Getters and setters
    void addValue(const std::string& value);
    const std::string& getValue(size_t index) const;
    std::string& getValue(size_t index);
    size_t getSize() const;

    // Convert to/from CSV format
    std::string toCSV() const;
    static Record fromCSV(const std::string& line);
};

#endif // RECORD_H
