#include "COLUMN_H"
#include <sstream>
#include <iostream>
#include <sstream>
#include <cstdlib>  // strtol, strtod
#include <cerrno>   // errno, ERANGE
#include <cctype>   // isspace
#include <climits>   // INT_MAX, INT_MIN

Column::Column(std::string name, size_t width, Column::DataType type) {
    if (name.size() > 10){
        this->name = name.substr(0, 10);
    }else{
        this->name = name;
    }

    this->width = width;
    this->type = type;
}

void Column::setWidth(size_t width){
    this->width = width;
}

const size_t Column::getWidth() const {
    return width;
}

void Column::setType(Column::DataType type){
    this->type = type;
}

const Column::DataType Column::getType() const {
    return type;
}

std::string Column::columnInfo() const {
    std::ostringstream ss;
    ss << name << " " << DataTypeName(type) << '(' << width << ')';
    return ss.str();
}

std::string Column::Trim(const std::string& s) const {
    std::string::size_type start = 0;
    while (start < s.size() && std::isspace((unsigned char)s[start])) {
        ++start;
    }

    std::string::size_type end = s.size();
    while (end > start && std::isspace((unsigned char)s[end - 1])) {
        --end;
    }

    return s.substr(start, end - start);
}

Column::DataType Column::DetectValueType(const std::string& input){
    std::string s = Trim(input);
    if (s.empty()) {
        return Column::STRING;
    }

    // 1) Integer check
    errno = 0;
    char* endInt = 0;
    (void)std::strtol(s.c_str(), &endInt, 10);

    if (endInt != s.c_str() && *endInt == '\0' && errno != ERANGE) {
        return Column::INTEGER;
    }

    // 2) Float check
    errno = 0;
    char* endFloat = 0;
    (void)std::strtod(s.c_str(), &endFloat);

    if (endFloat != s.c_str() && *endFloat == '\0' && errno != ERANGE) {
        return Column::FLOAT;
    }

    // 3) Not numeric
    return Column::STRING;
}

const std::string Column::DataTypeName(Column::DataType t) const {
    if (t == Column::STRING) {
        return "STRING";
    }
    if (t == Column::INTEGER) {
        return "INTEGER";
    }
    return "FLOAT";
}


//  int main() {
//     Column col("ColumnName");
//     col.setWidth(20);
//     std::cout << col.columnInfo() << std::endl;
//     col.setType(col.DetectValueType("123.45"));
//     std::cout << col.columnInfo() << std::endl;
//     col.setType(col.DetectValueType("123.45g"));
//     std::cout << col.columnInfo() << std::endl;
//     return 0;
// }
