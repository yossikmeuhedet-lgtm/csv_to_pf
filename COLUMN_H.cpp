#ifndef COLUMN_H
#define COLUMN_H

#include <string>

class Column {
    public:
        enum DataType {
            INTEGER,
            FLOAT,
            STRING
        };
    private:
        std::string name;
        size_t width;
        size_t precision;
        size_t wholePart;
        DataType type;
        const std::string DataTypeName(DataType t) const;
    public:
        Column(std::string name, size_t width = 0, DataType type = INTEGER);
        void setWidth(size_t width);
        const size_t getWidth() const;
        void setPrecision(size_t precision);
        const size_t getPrecision() const;
        void setType(DataType type);
        const DataType getType() const;
        const size_t getWholePart() const;
        static DataType DetectValueType(const std::string& input);
        static size_t DetectPrecision(const std::string& input);
        std::string columnInfo() const;
        static std::string Trim(const std::string& s);
};

#endif
