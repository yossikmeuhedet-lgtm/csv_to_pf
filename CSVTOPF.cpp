// CSVTOPF.CPP
// Description: Converts CSV files to PF (Physical File) format for IBM i
// Created: 2026-06-07
//
// Usage:
// CALL PGM(YOSSIK/CSVTOPF)
//      PARM(('/temp/YOSSIK/ambulance.csv', 'output_buffer'))
//
// Build commands:
// CRTCPPMOD MODULE(YOSSIK/COLUMN)  SRCFILE(YOSSIK/QCPPSRC) SRCMBR(COLUMN)
//
// CRTCPPMOD MODULE(YOSSIK/CSVTOPF) SRCFILE(YOSSIK/QCPPSRC) SRCMBR(CSVTOPF)
//
// CRTPGM PGM(YOSSIK/CSVTOPF) MODULE(YOSSIK/CSVTOPF YOSSIK/COLUMN)

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctype.h>
#include <cstring>
#include <cstdio>
#include "COLUMN_H"


static bool IsAlnumChar(char c) {
    return isalnum((unsigned char)c) != 0;
}

static bool IsDigitChar(char c) {
    return isdigit((unsigned char)c) != 0;
}

static std::vector<std::string> ParseCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    bool in_quotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            if (in_quotes && (i + 1) < line.size() && line[i + 1] == '"') {
                current += '"';
                ++i;
            } else {
                in_quotes = !in_quotes;
            }
        } else if (c == ',' && !in_quotes) {
            fields.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }

    fields.push_back(current);
    return fields;
}

static std::string columns_str(const std::vector<Column>& columns) {
    std::ostringstream ss;
    for(size_t i = 0; i < columns.size(); ++i) {
        ss << columns[i].columnInfo();
        if (i < columns.size() - 1) {
            ss << ", ";
        }
    }
    return ss.str();
}


static void copyToArgv1(const std::string& src, char argv1[2048])
{
    // Step 1: clear buffer
    std::memset(argv1, ' ', 2048);


    std::strncpy(argv1, src.c_str(), 2047);

    // Ensure null termination (important!)
    argv1[2047] = '\0';

}


// Returns a double-quoted, safe SQL identifier for DDL use.
// Strips everything except [A-Za-z0-9_], caps at 30 chars,
// prefixes with _ if the first char is a digit.
static std::string SanitizeColumnName(const std::string& raw) {
    std::string clean;
    for (size_t i = 0; i < raw.size(); ++i) {
        char c = raw[i];
        if (IsAlnumChar(c) || c == '_') {
            clean += c;
        } // everything else (spaces, /, -, ;, quotes, etc.) is dropped
    }

    if (clean.empty()) {
        clean = "COL";
    }

    if (IsDigitChar(clean[0])) {
        clean = "_" + clean; // SQL identifiers must not start with a digit
    }

    if (clean.size() > 10) {
        clean = clean.substr(0, 10); // traditional PF field name limit is 10
    }
    // Double-quote to make it a delimited identifier
    return clean;
}

static int ProcessCsvFile(const std::string& input_path, std::string& result) {
    std::ifstream input_file(input_path.c_str());
    if(!input_file.is_open()){
        std::cerr << "Error opening input file: " << input_path << std::endl;
        return 2;
    }

    std::string line;
    std::vector<Column> columns;
    std::string cell;

    // Read the header line to get column names
    if(std::getline(input_file, line)){
        std::vector<std::string> header_cells = ParseCsvLine(line);
        for (size_t i = 0; i < header_cells.size(); ++i) {
            columns.push_back(Column(SanitizeColumnName(header_cells[i])));
        }
    }

    while(std::getline(input_file, line)){
        std::vector<std::string> row_cells = ParseCsvLine(line);
        size_t i = 0;
        while(i < row_cells.size()){
            cell = row_cells[i];
            if(i < columns.size()){
                // Infer data type and update if necessary
                Column::DataType type = Column::DetectValueType(cell);
                if(type > columns[i].getType()){
                    columns[i].setType(type);
                }
                if(columns[i].getType() == Column::FLOAT){
                    size_t cell_precision = Column::DetectPrecision(cell);
                    size_t cell_width = 0;
                    for (size_t j = 0; j < cell.length(); ++j) {
                        if (cell[j] != '.') {
                            ++cell_width;
                        }
                    }
                    size_t cell_wholePart = 0;
                    if (cell_width > cell_precision) {
                        cell_wholePart = cell_width - cell_precision;
                    }
                    size_t current_wholePart = columns[i].getWholePart();

                    if(cell_width > columns[i].getWidth()){
                        columns[i].setWidth(cell_width);
                    }
                    if(cell_precision > columns[i].getPrecision()){
                        columns[i].setPrecision(cell_precision);
                    }
                    if(cell_wholePart > current_wholePart){
                        columns[i].setWidth(
                            cell_wholePart + columns[i].getPrecision()
                        );
                    }
                }
                else if(cell.length() > columns[i].getWidth()){
                    columns[i].setWidth(cell.length());
                }
            }
            ++i;
        }
    }

    input_file.close();
    result = columns_str(columns);

    return 0;
}



int main(int argc, char* argv[]){
    // must pass 2 arguments: input file, empty output buffer (char[2048])
    if(argc != 3){
        std::cerr << "Usage: " << argv[0] <<
        " <input_file> " << std::endl;
        return 1;
    }

    std::string result;
    std::string input_path = Column::Trim(argv[1]);

    size_t pos = input_path.find(' ');

    input_path = (pos == std::string::npos)
                      ? input_path
                      : input_path.substr(0, pos);

    int rc = ProcessCsvFile(input_path, result);
    if (rc != 0) {
        return rc;
    }

    copyToArgv1(result, argv[2]); // copy result to output buffer
    return 0;
}
