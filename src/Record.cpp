#include "Record.h"
#include "Utils.h"

Record::Record() {}

Record::Record(const std::vector<std::string>& vals)
    : values(vals) {}

void Record::addValue(const std::string& value) {
    values.push_back(value);
}

const std::string& Record::getValue(size_t index) const {
    if (index >= values.size()) {
        static std::string empty = "";
        return empty;
    }
    return values[index];
}

std::string& Record::getValue(size_t index) {
    return values[index];
}

size_t Record::getSize() const {
    return values.size();
}

std::string Record::toCSV() const {
    return Utils::join(values, ",");
}

Record Record::fromCSV(const std::string& line) {
    std::vector<std::string> values = Utils::split(line, ',');
    return Record(values);
}
