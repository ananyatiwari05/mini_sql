#ifndef RECORD_H
#define RECORD_H

#include <vector>
#include <string>

class Record {
private:
    std::vector<std::string> values;

public:
    Record();
    Record(const std::vector<std::string>& vals);

    void addValue(const std::string& value);
    const std::string& getValue(size_t index) const;
    std::string& getValue(size_t index);
    
    void setValue(size_t index, const std::string& value);
    
    size_t getSize() const;

    std::string toCSV() const;
    static Record fromCSV(const std::string& line);
};

#endif // RECORD_H
