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
    this->precision = 0;
    this->type = type;
}

void Column::setWidth(size_t width){
    this->width = width;
}

const size_t Column::getWidth() const {
    return width;
}

void Column::setPrecision(size_t precision){
    this->precision = precision;
}

const size_t Column::getPrecision() const {
    return precision;
}

void Column::setType(Column::DataType type){
    this->type = type;
}

const Column::DataType Column::getType() const {
    return type;
}

const size_t Column::getWholePart() const {
    return width - precision;
}

std::string Column::columnInfo() const {
    std::ostringstream ss;
    ss << name << " " << DataTypeName(type);
    if (type == Column::FLOAT) {
        ss << '(' << width << ',' << precision << ')';
    }
    else {
        ss << '(' << width << ')';
    }
    return ss.str();
}

std::string Column::Trim(const std::string& s) {
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

size_t Column::DetectPrecision(const std::string& input){
    std::string s = Trim(input);
    if (s.empty()) {
        return 0;
    }

    errno = 0;
    char* endFloat = 0;
    (void)std::strtod(s.c_str(), &endFloat);

    if (endFloat == s.c_str() ||
        *endFloat != '\0' ||
        errno == ERANGE) {
        return 0;
    }

    std::string::size_type expPos = s.find_first_of("eE");
    std::string::size_type mantissaEnd =
        (expPos == std::string::npos) ? s.size() : expPos;
    std::string::size_type dotPos = s.find('.');

    if (dotPos == std::string::npos || dotPos >= mantissaEnd) {
        return 0;
    }

    size_t precision = 0;
    for (std::string::size_type i = dotPos + 1;
         i < mantissaEnd;
         ++i) {
        if (!std::isdigit((unsigned char)s[i])) {
            return 0;
        }
        ++precision;
    }

    return precision;
}

const std::string Column::DataTypeName(Column::DataType t) const {
    if (t == Column::STRING) {
        return "CHAR";
    }
    return "DEC";
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
