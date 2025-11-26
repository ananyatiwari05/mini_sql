#include "Record.h"
#include "Utils.h"
using namespace std;

Record::Record() {}

Record::Record(const vector<string>& vals)
    : values(vals) {}

void Record::addValue(const string& value) {
    values.push_back(value);
}
void Record::setValue(int index, const string& newValue) {
    if (index >= 0 && index < values.size()) {
        values[index] = newValue;
    }
}
const string& Record::getValue(size_t index) const {
    if (index >= values.size()) {
        static string empty = "";
        return empty;
    }
    return values[index];
}

string& Record::getValue(size_t index) {
    return values[index];
}

size_t Record::getSize() const {
    return values.size();
}

string Record::toCSV() const {
    return Utils::join(values, ",");
}

Record Record::fromCSV(const string& line) {
    vector<string> values = Utils::split(line, ',');
    return Record(values);
}
