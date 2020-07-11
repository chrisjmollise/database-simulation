/*!
 * \file Table.cpp
 *
 * \author Christopher Mollise
 * \date 5/6/2020
 *
 * Project 3 for CS457
 *
 * This file is implementation for the Table class. This class contains all
 * relevant information about a Table including a list of its column elements
 * and the labels/keys for each row. The class handles read and write of
 * the table to and from its corresponding file.
 *
 */

#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include "../include/Table.h"
#include "../include/Column.h"

///std name space for general use
using namespace std;

/*!
 * \brief Constructor of the Table class
 *
 * This function will construct the Table class by reading its corresponding
 * file if it exists. If not it will create the default empty class. Each
 * table file contains a column and row size that will aid in the construction
 * of the table and accelerate file input. The files will also contain a list
 * of each column's information followed by its individual information.
 *
 * \param[in] string path
 *            String corresponding to the path that the tables file will be at
 */
Table::Table(string path) {
	rowNum = 0;
	rowAdjust = 0;

    string input; ///String containing input from file
	string newColName; ///String containing the columns name
	string newColType; ///String containing the columns type
	int newColSize; ///Int containing the column size if applicable
	size_t colNum; ///Int containing the number of columns

	/** Save Path for Later User **/
	tablePath = path;

	/** Create ifstream based on the path parameter **/
	ifstream inputFile(path);

	/** Check if the file exists and open **/
	if(inputFile.is_open()){
        /** Get column and row size from file **/
		inputFile >> colNum;
		inputFile >> rowNum;
        /** Input column settings based on number of columns **/
		for (size_t i = 0; i < colNum; i++) {
			inputFile >> newColName;
			inputFile >> newColType;
			/** Input the individual size of a column if applicable **/
			if (newColType == "char" || newColType == "varchar") {
				inputFile >> newColSize;
			}
			else ///Set the column size to 0 if an int or float
				newColSize = 0;
            /** Construct a new column and add to list from settings **/
			columns.push_back(Column(newColName, newColType, newColSize));
		}
        /** Get column information based on the row number **/
		for (size_t i = 0; i < rowNum; i++) {
            /**Go through each column pushing info for each row **/
			for (size_t j = 0; j < colNum; j++) {
				inputFile >> input;
				columns[j].colData.push_back(input);
			}
		}
		/** Close file **/
		inputFile.close();
	}
	/** Get and Add Table Path **/
	path = path.substr(path.find('/') + 1);
	path = path.substr(path.find('/') + 1);
	path = path.substr(0,path.find('.'));
	tableName = path;
	lowerName = tableName;
	transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
}
/*!
 * \brief Deconstructor of the Table class
 *
 * This function will deconstruct the Table class. This currently has no
 * effect on the class.
 *
 */
Table::~Table() {

}
/*!
 * \brief File writer for the Table class
 *
 * This function will write the Table class to its corresponding file. This
 * function works similar to the Constructor excepts it is writing the files
 * instead of reading from it. The function utilizes the sizes of the
 * existing row and column lists to provide it for the next constructing
 * of the database objects.
 *
 * \param[in] string path
 *            String corresponding to the path that the tables file will be at
 */
void Table::writeTable(string path) {
	bool printLine  = true;

    /** Create ofstream based on the path parameter **/
	ofstream outputFile(path);

	/** Check if the file exists and open **/
	if(outputFile.is_open()){
        /** Output file column and row size **/
		outputFile << columns.size() << " " << rowNum - rowAdjust<< endl;
        /** Go through each column and write setting information **/
		for (auto outputCol: columns) {
			outputFile << outputCol.colName << " " << outputCol.colType;
			/** Change output to include size if a char type **/
			if (outputCol.colType == "char" || outputCol.colType == "varchar") {
				outputFile << " " <<  outputCol.colSize;
			}
			outputFile << " ";
		}
		outputFile << endl;
		/** Go through each row and write data at each column **/
		for (size_t i = 0; i < rowNum; i++) {
			for (auto outputCol: columns) {
				/** Adjust printing to not print erased info **/
				if (outputCol.colData[i] != "ERASE" ) {
					outputFile << outputCol.colData[i];
					outputFile << " ";
				}
				else
					printLine = false;
			}
			if (i != rowNum - 1 && printLine)
				outputFile << endl;
			else
				printLine = true;
		}
		outputFile.close();
	}
}
/*!
 * \brief Method for the select table function
 *
 * This function will select the Table class. This function works by
 * reading out the individual settings of each column.
 *
 */
void Table::select() {
	bool firstInput = true; ///Boolean to adjust parser based on first input
	string tempInput;
	/** Iterate Through Each Column and Print **/
	for (auto inputColumn: columns)
	{
		if (firstInput) ///Do not print dividers if first input
			firstInput = false;
		else ///Print dividers
			cout << "|";
		cout << inputColumn.colName << " " << inputColumn.colType;
		/** If char print the column size **/
		if (inputColumn.colType == "char" || inputColumn.colType == "varchar")
			cout << "(" << inputColumn.colSize << ")";
	}
	cout << endl;
	firstInput = true;
	for (size_t i = 0; i < rowNum; i++) {
		for (auto outputCol: columns) {
			if (firstInput) ///Do not print dividers if first input
				firstInput = false;
			else ///Print dividers
				cout << "|";
			tempInput = outputCol.colData[i];
			if (outputCol.colType == "char" || outputCol.colType == "varchar") {
				tempInput.erase(0,1);
				tempInput.pop_back();
			}
			cout << tempInput;
		}
		firstInput = true;
		cout << endl;
	}
}
/*!
 * \brief Method for the alter table function
 *
 * This function will alter a table in the internal table list and then
 * replicate this creation on a file level. This  function will create
 * a new column including a new name, type, and size and add it to the
 * existing table. If the command fails it outputs back to the main parser
 * as usual.
 *
 * \return boolean true/false
 *         True or false value depending on state of function. False only sent
 *         if the parser cannot interpret the error and will send true in case
 *         of a processed create command if it is created or not.
 */
bool Table::alter() {
	string input; ///String containing input from file
	string newColName; ///String containing the columns name
	string newColType; ///String containing the columns type
	int newColSize; ///Int containing the column size if applicable

	/** Receive and Check Input From User **/
	cin >> newColName;
	cin >> newColType;
	if (newColType.back() != ';') ///Unknown command if no ";" at end of function
		return false;

	/** Process Input to Remove ";" **/
	newColType.pop_back();

	/** Check if token is of type char or varchar **/
	if (newColType.substr(0, newColType.find('(')) == "char" ||
		newColType.substr(0, newColType.find('(')) == "varchar") {
		/** Process the token to get column type **/
		input = newColType;
		newColType = input.substr(0, input.find('('));
		/** Process token to get the column size **/
		input = input.substr(input.find('(') + 1);
		input = input.substr(0, input.find(')'));
		newColSize = stoi(input);
	}
	else if (newColType == "int" || newColType == "float")
		newColSize = 0;
	else ///If not valid error
		return false;
	/** Add Column onto Table **/
	columns.push_back(Column(newColName, newColType, newColSize));
	/** Add Valid Path to Table **/
	writeTable(tablePath);
	cout << "Table "
		 << tableName
		 << " modified." << endl;
	return true; ///Return true for end of processing
}
/*!
 * \brief Method for the insert table function
 *
 * This function will insert into a table in the internal table list and then
 * replicate this creation on a file level. This  function will create
 * a new row including relevant information and add it to the
 * existing table. If the command fails it outputs back to the main parser
 * as usual.
 *
 * \return boolean true/false
 *         True or false value depending on state of function. False only sent
 *         if the parser cannot interpret the error and will send true in case
 *         of a processed create command if it is created or not.
 */
bool Table::insert() {
	string input; ///String containing input from file
	bool firstInput = true; ///Bool checking if still on intial input

	cin >> input;
	if (input.back() == ';')
		return insertLine(input);
	/** Iterate through columns pushing information to each column **/
	for (size_t i = 0; i < columns.size(); i++) {
		if (!firstInput)
			cin >> input; ///Get initial input
		else
			firstInput = false;
		if (i == 0) ///Additional editing on first input
		{
			///Remove the "values(" in input
			if (input.rfind("values(",0) != 0)
				return false;
			else {
				input.erase(0, 7);
				///After erasing at to column
				if (input.back() != ',')
					return false;
				else {
					input.pop_back();
					columns[i].colData.push_back(input);
				}
			}
		}
		/** Additional editing if last input **/
		else if (i == columns.size() - 1) {
			///Check for the  ';' char
			if (input.back() != ';') {
				///Remove invalid data
				for (size_t j = 0; j < i; j++)
					columns[j].colData.pop_back();
				return false;
			}
			else {
				///Remove the ; and check for ')'
				input.pop_back();
				if (input.back() != ')') {
					///Remove invalid data
					for (size_t j = 0; j < i; j++)
						columns[j].colData.pop_back();
					return false;
				}
				else ///Add correct information to column
				{
					input.pop_back();
					columns[i].colData.push_back(input);
				}
			}
		}
		/** Checking for standard ',' seperated input **/
		else {
			/** Check for ',' **/
			if (input.back() != ',') {
				///Remove invalid data
				for (size_t j = 0; j < i; j++)
					columns[j].colData.pop_back();
				return false;
			}
			else ///Add correct information to column
			{
				input.pop_back();
				columns[i].colData.push_back(input);
			}
		}
	}
	/** Increase row amount and print output **/
	rowNum++;
	writeTable(tablePath);
	cout << "1 new record inserted." << endl;
	return true; ///Return true for end of processing
}
/*!
 * \brief Method for the insert line function
 *
 ** This function will insert into a table in the internal table list and then
 * replicate this creation on a file level. This  function will create
 * a new row including relevant information and add it to the
 * existing table. If the command fails it outputs back to the main parser
 * as usual. This functions different from the insert function due to the
 * parser syntax used.
 *
 * \param[in] string input
 *            String corresponding to the token input found in orginal
 *            insert parser.
 *
 * \return boolean true/false
 *         True or false value depending on state of function. False only sent
 *         if the parser cannot interpret the error and will send true in case
 *         of a processed create command if it is created or not. This function
 *         will additionally detect if the user is selecting all or selecting a
 *         particular column and direct to the correct function.
 */
bool Table::insertLine (string input) {
	/** Remove 'values(' in string **/
	if (input.find("values(") != string::npos)
		input = input.substr(input.find('(') + 1);
	else
		return false;
	/** Iterate through columns pushing information to each column **/
	for (size_t i = 0; i < columns.size(); i++) {
		if (input.find(',') != string::npos) {
			columns[i].colData.push_back(input.substr(0,input.find(',')));
			input = input.substr(input.find(',')+1);
		}
		else {
			input.pop_back();
			if(input.back() != ')') {
				///Remove invalid data
				for (size_t j = 0; j < i; j++)
					columns[j].colData.pop_back();
				return false;
			}
			else {
				input.pop_back();
				columns[i].colData.push_back(input);
			}
		}
	}
	/** Increase row amount and print output **/
	rowNum++;
	writeTable(tablePath);
	cout << "1 new record inserted." << endl;
	return true; ///Return true for end of processing
}
/*!
 * \brief Method for the update table function
 *
 * This function will update the table in the internal table list and then
 * replicate this update on a file level. This  function will edit
 * an existing row including chosen information and add it to the
 * existing table. If the command fails it outputs back to the main parser
 * as usual.
 *
 * \return boolean true/false
 *         True or false value depending on state of function. False only sent
 *         if the parser cannot interpret the error and will send true in case
 *         of a processed create command if it is created or not.
 */
bool Table::update() {
	string setName; ///String holds the name of what value will be set
	string whereName; ///String holds the name of where the value will be set
	string setValue; ///String Holds the value of the information after the change
	string whereValue; ///String Holds the value of the information to change or compare
	string input; ///String holds all input
	int recordCount = 0; ///Int holds the number of records changed

	/** Get Set Input From User **/
	cin >> input;
	if (input != "set") ///Unknown command if no "set"
		return false;
	cin >> setName;
	cin >> input;
	if (input != "=") ///Unknown command if no '='
		return false;
	cin >> setValue;

	/** Get Where Input From User **/
	cin >> input;
	if (input != "where") ///Unknown command if no "where
		return false;
	cin >> whereName;
	cin >> input;
	if (input != "=") ///Unknown command if no "="
		return false;
	cin >> whereValue;
	if (whereValue.back() != ';') ///Unknown command if no ";" at end of function
		return false;

	/** Process Input to Remove ";" **/
	whereValue.pop_back();

	/** Go through table looking for values and replacing as nessecary **/
	for (size_t i = 0; i < columns.size(); i++) ///Go through each column
	{
		if (columns[i].colName == whereName) ///Continue search if column name matches
		{
			for (size_t j = 0; j < columns[i].colData.size(); j++) ///Go through each row
			{
				if (columns[i].colData[j] == whereValue) ///Change if column value matches
				{
					/** Go Through Each Column Changing Value If Matching and Increase Count **/
					for (size_t k = 0; k < columns.size(); k++){
						if (columns[k].colName == setName) {
							columns[k].colData[j] = setValue;
							recordCount++;
						}
					}
				}
			}
		}
	}

	/** Rewrite Table **/
	writeTable(tablePath);

	/** Print Output **/
	if (recordCount == 1)
		cout << "1 record modified." << endl;
	else
		cout << recordCount << " records modified." << endl;

	return true; ///Return true for end of processing
}
/*!
 * \brief Method for the delete from table function
 *
 * This function will delete from the table in the internal table list and then
 * replicate this update on a file level. This  function will delete
 * an existing row based on chosen information and remove it from the
 * existing table. If the command fails it outputs back to the main parser
 * as usual.
 *
 * \return boolean true/false
 *         True or false value depending on state of function. False only sent
 *         if the parser cannot interpret the error and will send true in case
 *         of a processed create command if it is created or not.
 */
bool Table::deleteFrom() {
	string whereName; ///String holds the name of where the value will be set
	string whereValue; ///String Holds the value of the information to change or compare
	char deleteType; ///Char holds the type of information deleted
	string input; ///String holds all input
	int deleteHere = -1; ///Int holds the index of a deleted row
	int recordCount = 0; ///Holds the number of records
	bool deleted = false; ///Holds if something has been deleted

	/** Get Input **/
	cin >> input;
	if (input != "where") ///Unknown command if no "where"
		return false;
	cin >> whereName;
	cin >> deleteType;
	cin >> whereValue;
	if (whereValue.back() != ';') ///Unknown command if no ";" at end of function
		return false;

	/** Process Input to Remove ";" **/
	whereValue.pop_back();

	/** Loop through the system deleting rows when information matches **/
	do {
		deleted = false; ///Reset the deleted row status
		/** Loop through each column **/
		for (size_t i = 0; i < columns.size(); i++){
			/** Go to column if name matches **/
			if (columns[i].colName == whereName) {
				/** Go through each row **/
				for (size_t j = 0; j < columns[i].colData.size(); j++){
					/** Switch based on the delete type **/
					if (deleteType == '=') {
						/** Change index and status if a delete row is found **/
						if (columns[i].colData[j] != "ERASE") {
							if (columns[i].colData[j] == whereValue) {
								deleteHere = j;
								deleted = true;
								break;
							}
						}
					}
					/** Change index and status if a delete row is found **/
					else if (deleteType == '>') {
						if (columns[i].colData[j] != "ERASE") {
							if (stof(columns[i].colData[j]) > stof(whereValue)) {
								deleteHere = j;
								deleted = true;
								break;
							}
						}
 					}
				}
			}
		}
		/** If deleted go through row and change value to "ERASE" and increase count**/
		if (deleted) {
			for (size_t i = 0; i < columns.size(); i++) {
				columns[i].colData[deleteHere] = "ERASE";
			}
			recordCount++;
		}
	} while (deleted); ///Loop as long as something deletable is found to get all

	/**Rewrite table and reread into system after a delete **/
	rowAdjust = recordCount;
	writeTable(tablePath);
	reInput();

	/** Output information **/
	if (recordCount == 1)
		cout << "1 record deleted." << endl;
	else
		cout << recordCount << " records deleted." << endl;
	return true; ///Return true for end of processing
}
/*!
 * \brief Reinput Method for the Table class
 *
 * This function will rewrite the Table class to the database. This
 * function works similar to the Constructor excepts it is rereading from files
 * with an existing database. The function utilizes the sizes of the
 * existing row and column lists to provide it for the next constructing
 * of the database objects.
 *
 */
void Table::reInput() {
	rowAdjust = 0; ///Resets the adjustment amount
	columns.clear(); ///Erases all columns in system

	string input; ///String containing input from file
	string newColName; ///String containing the columns name
	string newColType; ///String containing the columns type
	int newColSize; ///Int containing the column size if applicable
	size_t colNum; ///Int containing the number of columns

	/** Create ifstream based on the path parameter **/
	ifstream inputFile(tablePath);

	/** Check if the file exists and open **/
	if(inputFile.is_open()){
		/** Get column and row size from file **/
		inputFile >> colNum;
		inputFile >> rowNum;
		/** Input column settings based on number of columns **/
		for (size_t i = 0; i < colNum; i++) {
			inputFile >> newColName;
			inputFile >> newColType;
				/** Input the individual size of a column if applicable **/
				if (newColType == "char" || newColType == "varchar") {
					inputFile >> newColSize;
				}
				else ///Set the column size to 0 if an int or float
					newColSize = 0;
	            /** Construct a new column and add to list from settings **/
				columns.push_back(Column(newColName, newColType, newColSize));
			}
	        /** Get column information based on the row number **/
			for (size_t i = 0; i < rowNum; i++) {
	            /**Go through each column pushing info for each row **/
				for (size_t j = 0; j < colNum; j++) {
					inputFile >> input;
					columns[j].colData.push_back(input);
				}
			}
			/** Close file **/
			inputFile.close();
		}
}
/*!
 * \brief Method for the select types table function
 *
 * This function will select the Table class. This function works by
 * reading out the individual settings of each column that is specified
 * by a passed type.
 *
 * \param[in] vector <string> types
 *            Strings holding the names of columns to print
 */
bool Table::selectTypes(vector <string> types) {
	bool firstInput = true; ///Boolean to adjust parser based on first input
	bool printed = false; ///Boolean holding if something has been printed yet
	bool printColumn = false; ///Boolean holding if a column should be printed
	string input; ///String holds all input
	string tempInput; ///String Temporary input to adjust for char printing
	string whereName; ///String Name of where information should print
	string selectType; ///String Holds what type of comparision to make
	string whereValue; ///String Holds value to compare to
	int whereIndex; ///Holds index of where a value has been found

	/** Get Input **/
	cin >> input;
	if (input != "where") ///Unknown command if no "where"
		return false;
	cin >> whereName;
	cin >> selectType;
	cin >> whereValue;

	if (whereValue.back() != ';') ///Unknown command if no ";" at end of function
		return false;

	/** Process Input to Remove ";" **/
	whereValue.pop_back();

	/**Go through each column to get index of matching name **/
	for (size_t i = 0; i < columns.size(); i++) {
		if (columns[i].colName == whereName)
			whereIndex = i;
	}

	/** Iterate Through Each Column and Print **/
	for (auto inputColumn: columns) {
		for (size_t i = 0; i < types.size(); i++) {
			if (types[i] == inputColumn.colName) ///Print only columns of matching name
				printColumn = true;
		}
		if (printColumn) ///Print only if a printable column
		{
			if (firstInput) ///Do not print dividers if first input
				firstInput = false;
			else ///Print dividers
				cout << "|";
			cout << inputColumn.colName << " " << inputColumn.colType;
			/** If char print the column size **/
			if (inputColumn.colType == "char" || inputColumn.colType == "varchar")
				cout << "(" << inputColumn.colSize << ")";
			printColumn = false; ///Reset print status
		}
	}

	cout << endl;
	firstInput = true; ///Reset first input status

	/** Iterate Through Each Row and Print **/
	for (size_t i = 0; i < rowNum; i++) {
		for (auto outputCol: columns) {
			for (size_t i = 0; i < types.size(); i++) {
				if (types[i] == outputCol.colName) ///Print only columns of matching name
					printColumn = true;
			}
			if (printColumn) ///Print only if a printable column
			{
				if (firstInput && !printed) ///Do not print dividers if first input
					firstInput = false;
				else if (printed)///Print dividers
					cout << "|";
				/** Adjust printout if the column is of type char or varchar **/
				tempInput = outputCol.colData[i];
				if (outputCol.colType == "char" || outputCol.colType == "varchar") {
					tempInput.erase(0,1);
					tempInput.pop_back();
				}
				/** Print based on the print type, can be changed in future interations **/
				if (stoi(columns[whereIndex].colData[i]) != stoi(whereValue)) {
					cout << tempInput;
					printed = true; ///Change status to printed if something has passed
				}
				printColumn = false; ///Reset printColumn status
			}
		}
		if (printed) ///Only print an endl if something was printed in that row
			cout << endl;
		/** Reset variables for a new row **/
		printed = false;
		firstInput = true;
	}
	return true; ///Return a sucessful print
}
