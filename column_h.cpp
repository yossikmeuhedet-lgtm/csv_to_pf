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
        DataType type;
        const std::string DataTypeName(DataType t) const;
        static std::string Trim(const std::string& s);
    public:
        Column(std::string name, size_t width = 0, DataType type = INTEGER);
        void setWidth(size_t width);
        const size_t getWidth() const;
        void setType(DataType type);
        const DataType getType() const;
        static DataType DetectValueType(const std::string& input);
        std::string columnInfo() const;
};

#endif
