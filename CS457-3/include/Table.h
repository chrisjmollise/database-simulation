/*!
 * \file Table.h
 *
 * \author Christopher Mollise
 * \date 5/6/2020
 *
 * Project 3 for CS457
 *
 * Header for Table Class
 *
 */


#ifndef TABLE_H
#define TABLE_H

#include <string>
#include <vector>

#include "Column.h"

/*!
 * \class Table
 *
 * \brief This class acts as a virtual implementation of a table
 *
 * This class holds the information for a table within a database.
 * This virtual table will allow for easier data manipulation as
 * the data will be already loaded in the program.
 *
 * \author Christopher Mollise
 * \date 5/6/20
 */
class Table {
public:
    Table(std::string path); ///Default constructor
    virtual ~Table(); ///Default deconstructor
    void writeTable(std::string path); ///Function to write table to file
    bool alter(); ///Function to alter table columns
    void select(); ///Function to list table columns
    bool insert(); ///Function to insert into table columns
    bool insertLine(std::string input); ///Function if no spaces in insert command
    bool update(); ///Function to update table columns
    bool deleteFrom(); ///Function to delete input from tables
    void reInput(); ///Function to reload database after delete changes
    bool selectTypes(std::vector <std::string> types); ///Seperate select for different types

    std::vector<Column> columns; ///Vector of Column Objects
    std:: string lowerName; ///String containing lowercase tableName
    std:: string tableName; ///String containing tableName
    std::string tablePath; ///String containing tablePath
    size_t rowNum; ///Holds the number of rows in the column
    int rowAdjust; ///Holds an adjustment amount for reloading the file after delete
protected:
};

#endif // TABLE_H
