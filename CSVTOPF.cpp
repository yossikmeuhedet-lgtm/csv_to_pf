// CSVTOPF.CPP
// Description: Converts CSV files to PF (Physical File) format for IBM i
// Created: 2026-06-07
//
// Usage:
// CALL PGM(YOSSIK/CSVTOPF)
//      PARM(('/temp/YOSSIK/ambulance.csv'))
//
// creation:
// CRTCPPMOD MODULE(YOSSIK/COLUMN) SRCFILE(YOSSIK/QCPPSRC)
// CRTCPPMOD MODULE(YOSSIK/CSVTOPF) SRCFILE(YOSSIK/QCPPSRC)
// CRTPGM PGM(YOSSIK/CSVTOPF) MODULE(YOSSIK/CSVTOPF YOSSIK/COLUMN)

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "COLUMN_H"

void print_columns(const std::vector<Column>& columns) {
    for(size_t i = 0; i < columns.size(); ++i) {
        std::cout << columns[i].columnInfo() << std::endl;
    }
}

// void print_args(int argc, char* argv[]){
//     std::cout << "Arguments passed: " << argc << std::endl;
//     for(int i = 0; i < argc; ++i){
//         std::cout << "Argument " << i << ": " << argv[i] << std::endl;
//     }
// }

int main(int argc, char* argv[]){
    // must pass 1 arguments: input file
    if(argc != 2){
        std::cerr << "Usage: " << argv[0] <<
        " <input_file> " << std::endl;
        return 1;
    }

    std::ifstream input_file(argv[1]);
    if(!input_file.is_open()){
        std::cerr << "Error opening input file: " << argv[1] << std::endl;
        return 2;
    }

    std::string line;
    std::vector<Column> columns;
    std::string cell;

    // Read the header line to get column names
    if(std::getline(input_file, line)){
        std::stringstream header_ss(line);
        while(std::getline(header_ss, cell, ',')){
            columns.push_back(Column(cell));
        }
    }

    //std::vector<std::vector<std::string> > data;
    while(std::getline(input_file, line)){
        std::stringstream row_ss(line);
        std::vector<std::string> row;
        size_t i = 0;
        while(std::getline(row_ss, cell, ',')){
            row.push_back(cell);
            // Update column width if necessary
            if(i < columns.size()){
                if(cell.length() > columns[i].getWidth()){
                    columns[i].setWidth(cell.length());
                }
                // Infer data type and update if necessary
                Column::DataType type = Column::DetectValueType(cell);
                if(type > columns[i].getType()){
                    columns[i].setType(type);
                }
            ++i;
            }
        }
    }

    input_file.close();
    print_columns(columns);

    return 0;
}
