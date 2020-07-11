/*!
 * \file Database.h
 *
 * \author Christopher Mollise
 * \date 5/6/2020
 *
 * Project 3 for CS457
 *
 * Header for Database Class
 *
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>

#include "Table.h"

/*!
 * \class Database
 *
 * \brief This class acts as a virtual implementation of a database
 *
 * This class holds the information for a database within a directory.
 * This virtual database will allow for easier data manipulation as
 * the data will be already loaded in the program.
 *
 * \author Christopher Mollise
 * \date 5/6/20
 */
class Database {
public:
    Database(std::string path); ///Default Constructor
    virtual ~Database(); ///Default Deconstructor

    std::string dataBaseParser(); ///Function to act as secondary parser

    std::string actualName; ///String containing the actual database name
    std::string dataBaseName; ///String containing the usuable database name
protected:
    bool createTable(); ///Function to create table
    bool dropTable(); ///Function to drop table
    bool selectTable(); ///Function to select table
    bool innerJoin(std::string firstTable);///Function to inner join table
    bool outerJoin(std::string firstTable);///Function to outer join table
    bool alterTable(); ///Function to alter table
    bool insertTable(); ///Function to insert into table
    bool updateTable(); ///Function to update table
    bool deleteFromTable(); ///Function to delete from table

    std::vector <Table> tables; ///Vector containing Table objects
};

#endif // DATABASE_H
