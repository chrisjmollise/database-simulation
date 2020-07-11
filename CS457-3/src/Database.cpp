/*!
 * \file Database.cpp
 *
 * \author Christopher Mollise
 * \date 5/6/2020
 *
 * Project 3 for CS457
 *
 * This file controls all operations at a Database level. This includes the
 * capabilities to edit tables and their contents as well as perform joins.
 * The control function in this program is the dataBaseParser that handles
 * all commands not handled in the main parser. These include actions that
 * are table manipulations, making it easier to control the structure of the
 * table list due to less calls to class elements.
 *
 */
#include <bits/stdc++.h>
#include <experimental/filesystem>
#include <iostream>
#include <vector>

#include "../include/Database.h"
#include "../include/Table.h"

/// std name space for general use
using namespace std;

/// Name space for use for shortening the file system name space
/// Not using whole name space as it is not needed
namespace fs = std::experimental::filesystem;

/*!
 * \brief Constructor of the Database class
 *
 * This function will construct the Database class by reading its
 * corresponding directory if it exists. If not it will create the default
 * empty class. Each database contains its name and a list of Tables within
 * the directory
 *
 * \param[in] string path
 *            String corresponding to the path that the tables file will be at
 */
Database::Database(string path) {
	/** File Reader **/
	for(auto&p: fs::directory_iterator(path)) {
		tables.push_back(Table(p.path()));
	}
}
/*!
 * \brief Deconstructor of the Database class
 *
 * This function will deconstruct the Database class. This currently has no
 * effect on the class.
 *
 */
Database::~Database() {

}
/*!
 * \brief Function for the create table function
 *
 * This function will create a table in the internal table list and then
 * replicate this creation on a file level. When the main parser detects the
 * CREATE command and a database has been used it will direct to this
 * function. The function will then create a file based on the name and
 * create or deny it from the list if the file does or does not
 * exist. t will then output the appropriate error messages.The function
 * will then populate the internal and external table  if the provided
 * construction information for individual columns.
 *
 * \return boolean true/false
 *         True or false value depending on state of function. False only sent
 *         if the parser cannot interpret the error and will send true in case
 *         of a processed create command if it is created or not.
 */
bool Database::createTable() {
	string input; /// String holding user input
	string tableName; /// String containing the table name
	string colNameNoSpace; /// String to hold column name if no space between table name

	bool endInput = false; /// Boolean to determine if column input is done
	bool firstInput = true; /// Boolean to adjust parser based on first input
	bool noSpace = false; /// Boolean if no space between table and column name

	string newColName; /// String containing a new columns name
	string newColType; /// String containing a new columns type
	int newColSize; /// Int containing a new columns size

	fs::path dataPath = "Databases/"; ///Path for the table file

	/** Receive and Check Input From User **/
	cin >> input;
	if (input != "TABLE" && input != "table") /// Unknown command if TABLE not specified
		return false;
	cin >> input;
	if (input.find('(') != string::npos) {
		noSpace = true;
		colNameNoSpace = input.substr(input.find('('));
		input = input.substr(0, input.find('('));
	}

	/** Process path for detecting directory **/
	dataPath += dataBaseName + '/' + input + ".txt";

	/** Attempt to detect directory at path **/
	if (fs::exists(dataPath)) /// Detect and output error if found
	{
		cout << "!Failed to create table "
		     << input
		     << " because it already exists." << endl;
		cin.ignore();
		getline (cin, input);
	}
	else /// Create message if success and add database to internal list
	{
		/** Set Table Name for Later Use **/
		tableName = input;

		/** Push new Table onto List **/
		tables.push_back(Table(dataPath));

		/** Main Table Creator **/
		while (!endInput) /// Run until input ends / ";" found
		{
			/** Get Input **/
			if (!noSpace)
				cin >> newColName;
			else
			{
				noSpace = false;
				newColName = colNameNoSpace;
			}
			if (firstInput) /// Check for particular first input problems
			{
				/** Check for Empty Token **/
				if (newColName == "();") /// Handle an Empty Table
				{
					tables.back().writeTable(dataPath);
					cout << "Table "
						 << tableName
						 << " created." << endl;
					return true;
				}
				/** Remove the starting "(" **/
				newColName.erase(0,1);
				firstInput = false;
			}
			/** Get Column Type **/
			cin >> newColType;
			if (newColType.back() != ',' && newColType.back() != ';') /// Check for valid token ending "," or ";"
			{
				tables.pop_back();
				return false;
			}
			else /// Valid type token
			{
				/** Process Type Token **/
				if (newColType.back() == ';') /// If token ends with ";" it is last token
				{
					newColType.pop_back();
					endInput = true;
				}
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
				else if (newColType == "int" || newColType == "float") /// Check for other types
					newColSize = 0;
				else /// If not valid remove table
				{
					tables.pop_back();
					return false;
				}
				/** Add Column onto Table **/
				tables.back().columns.push_back(Column(newColName, newColType, newColSize));
			}
		}
		/** Add Valid Path to Table **/
		tables.back().writeTable(dataPath);
		cout << "Table "
			 << tableName
			 << " created." << endl;
	}
	return true; /// Return true for end of processing
}
/*!
 * \brief Function for the drop table function
 *
 * This function will drop a table from the internal Table list and then
 * replicate this deletion on a file level. When the main parser detects the
 * DROP command and a database has been used it will direct to this
 * function. The function will then drop a table based on the name and
 * drop or deny it from the table list if the file does or does not
 * exist. It will then output the appropriate error messages.
 *
 * \return boolean true/false
 *         True or false value depending on state of function. False only sent
 *         if the parser cannot interpret the error and will send true in case
 *         of a processed create command if it is created or not.
 */
bool Database::dropTable() {
	string input; /// String holding user input
	fs::path dataPath = "Databases/"; /// Directory path for deleted database

	/** Receive and Check Input From User **/
	cin >> input;
	if (input != "TABLE") /// Unknown command if TABLE not specified
		return false;
	cin >> input;
	if (input.back() != ';') /// Unknown command if no ";" at end of function
		return false;

	/** Process path for deleting file **/
	input.pop_back();
	dataPath += dataBaseName + '/' + input + ".txt";

	/** Attempt to delete directory at path **/
	if(!fs::remove_all(dataPath)) /// Delete and output error if fails
	{
		cout << "!Failed to delete "
			 << input
		     << " because it does not exist." << endl;
	}
	else /// Output message if success and delete table from internal list
	{
		/** Locate internal database to also remove **/
		for (size_t i = 0; i < tables.size(); i++) {
			if (tables[i].tableName == input)
				tables.erase(tables.begin() + i);
		}
		cout << "Table "
		     << input
		     << " deleted." << endl;
	}

	return true; /// Return true for end of processing
}
/*!
 * \brief Function for the select table function
 *
 * This function will select a table from the internal Table list.
 * When the main parser detects the SELECT command and a database
 * has been used it will direct to this function. The function will
 * then list all the column information or output the appropriate
 * error messages. This function also leads to the joining functions.
 *
 * \return boolean true/false
 *         True or false value depending on state of function. False only sent
 *         if the parser cannot interpret the error and will send true in case
 *         of a processed create command if it is created or not. This function
 *         will additionally detect if the user is selecting all or selecting a
 *         particular column and direct to the correct function.
 */
bool Database::selectTable() {
	string input; /// String holding user input
	vector <string> types; /// Boolean selected types if not all are selected
	bool typeBased = false; /// Boolean if all types are selected or not
	string tempTableName; /// String holds a temp name for joining

	/** Receive and Check Input From User **/
	cin >> input;
	if (input != "*") /// Switch if not all types selected
	{
		typeBased = true; /// Change type of function to use
		/** Receive input types **/
		if (input.back() == ',') /// Process first input to see if anymore
		{
			input.pop_back();
			types.push_back(input);
			/** Loop if more than one input checking for ',' **/
			while (true) {
				cin >> input;
				if (input.back() == ',')  /// Check if more inputs and process
				{
					input.pop_back();
					types.push_back(input);
				}
				else /// Get final input and leave loop
				{
					types.push_back(input);
					break;
				}
			}
		}
		else /// No more inputs after the first input
			types.push_back(input);
	}

	/** Process following input **/
	cin >> input;
	if (input != "FROM" && input != "from") /// Unknown command if TABLE not specified
		return false;
	cin >> input;
	/** Switch input based on function used **/
	if (!typeBased) {
		if (input.back() != ';') /// Switch if a join command instead of select all
		{
			tempTableName = input; /// Temporary storage for table one name
			/** Continue input if not a standard selection **/
			cin >> input;
			/** Switch to a regular join **/
			if (input.back() == ',')
				return innerJoin(tempTableName);
			else /// Check if a inner or outer join
			{
				/** Get input and check for an inner join **/
				cin >> input;
				if (input == "inner"){
					cin >> input;
					if (input == "join")
						return innerJoin(tempTableName);
					else /// Invalid command
						return false;
				}
				else if (input == "left"){
					/** Get input and check for an outer join **/
					cin >> input;
					if (input == "outer"){
						cin >> input;
						if (input == "join")
							return outerJoin(tempTableName);
						else /// Invalid command
							return false;
					}
					else /// Invalid command
						return false;
				}
				else /// Invalid command
					return false;
			}
		}
		/** Process input for table location **/
		input.pop_back();
	}

	/** Change input to lower case **/
	transform(input.begin(), input.end(), input.begin(), ::tolower);

	/** Locate Table in Internal List **/
	for (auto inputTable: tables){
		if (inputTable.lowerName == input) {
				/** Change function based on type of input **/
				if (typeBased)
					return inputTable.selectTypes(types);
				else
					inputTable.select();
				return true;
			}
		}

	/** Output error if location failed **/
	cout << "!Failed to query table "
		 << input
		 << " because it does not exist." << endl;

	return true; ///Return true for end of processing
}
/*!
 * \brief Function for the inner join table function
 *
 * This function will select two tables from the internal Table list.
 * When the main parser detects the INNER JOIN command and a database
 * has been used it will direct to this function. The function will
 * then list all the column information or output the appropriate
 * error messages. This function also leads to the joining functions.
 *
 * \param[in] string firstTable
 *            String corresponding name of the first table to join
 *
 * \return boolean true/false
 *         True or false value depending on state of function. False only sent
 *         if the parser cannot interpret the error and will send true in case
 *         of a processed create command if it is created or not. This function
 *         will additionally detect if the user is selecting all or selecting a
 *         particular column and direct to the correct function.
 */
bool Database::innerJoin(string firstTable) {
	string input; /// String holds input
	string secondTable; /// String holds the second table
	string firstType; /// String holds first compare type
	string secondType; /// String holds second compare type
	int firstTableIndex = -1; /// Int index of table 1
	int secondTableIndex = -1; /// Int index of table 2
	int firstTableTypeIndex = -1; /// Int index of table 1's search type
	int secondTableTypeIndex = -1;/// Int index of table 2's search type
	bool firstInput = true; /// Stores if first input to print

	/** Get second table name and process **/
	cin >> secondTable;
	cin >> input;
	cin >> input;

	/** Check for correct syntax **/
	if (input != "where" && input != "on")
		return false;
	cin >> input;

	/** Check if proper '.' format **/
	if (input.find('.') == string::npos)
		return false;

	/** Get first table column type **/
	firstType = input.substr(input.find('.') + 1);

	/** Check if proper '=' format **/
	cin >> input;
	if (input != "=")
		return false;

	/** Get second table column type **/
	cin >> input;
	if (input.find('.') == string::npos ) /// Check if proper '.' format
		return false;
	if (input.back() != ';') /// Check if ending in ';'
		return false;
	input.pop_back();
	secondType = input.substr(input.find('.') + 1);

	/** Change names to lower case **/
	transform(firstTable.begin(), firstTable.end(), firstTable.begin(), ::tolower);
	transform(secondTable.begin(), secondTable.end(), secondTable.begin(), ::tolower);

	/** Check if index exists for tables **/
	for (size_t i = 0; i < tables.size(); i++) /// Iterate through all tables
	{
		if (tables[i].lowerName == firstTable) /// Check if name matches table 1
			firstTableIndex = i;
		else if (tables[i].lowerName == secondTable) /// Check if name matches table 2
			secondTableIndex = i;
	}

	/** Check for failure **/
	if (firstTableIndex == -1 || secondTableIndex == -1)
		return false;

	/** Check if index exists for first table **/
	for (size_t i = 0; i < tables[firstTableIndex].columns.size(); i++)
		if (tables[firstTableIndex].columns[i].colName == firstType)
			firstTableTypeIndex = i;

	/** Check if index exists for first table **/
	for (size_t i = 0; i < tables[secondTableIndex].columns.size(); i++)
		if (tables[secondTableIndex].columns[i].colName == secondType)
			secondTableTypeIndex = i;

	/** Check for failure **/
	if (firstTableTypeIndex == -1 || secondTableTypeIndex == -1)
		return false;

	/** Print table 1 header **/
	for (auto inputColumn: tables[firstTableIndex].columns) {
		if (firstInput) ///Do not print dividers if first input
			firstInput = false;
		else ///Print dividers
			cout << "|";
		cout << inputColumn.colName << " " << inputColumn.colType;
		/** If char print the column size **/
		if (inputColumn.colType == "char" || inputColumn.colType == "varchar")
			cout << "(" << inputColumn.colSize << ")";
	}

	/** Print table 2 header **/
	for (auto inputColumn: tables[secondTableIndex].columns) {
		/** Print dividers and input **/
		cout << "|" << inputColumn.colName << " " << inputColumn.colType;
		/** If char print the column size **/
		if (inputColumn.colType == "char" || inputColumn.colType == "varchar")
			cout << "(" << inputColumn.colSize << ")";
	}
	firstInput = true;
	cout << endl;

	/** Print joined table **/
	for (size_t i = 0; i < tables[firstTableIndex].rowNum; i++) /// Iterate through table 1
	{
		/** Iterate through table 2 **/
		for (size_t j = 0; j < tables[secondTableIndex].rowNum; j++){
			/** Check to see if a matching tuple is found **/
			if (tables[firstTableIndex].columns[firstTableTypeIndex].colData[i] ==
				tables[secondTableIndex].columns[secondTableTypeIndex].colData[j]) {
				/** Loop through table 1 columns and print at tuple **/
				for (auto outputCol: tables[firstTableIndex].columns) {
					if (firstInput) ///Do not print dividers if first input
						firstInput = false;
					else ///Print dividers
						cout << "|";
					input = outputCol.colData[i];
					/** Adjust input for character columns **/
					if (outputCol.colType == "char" || outputCol.colType == "varchar") {
						input.erase(0,1);
						input.pop_back();
					}
					cout << input;
				}
				/** Loop through table 2 columns and print at tuple **/
				for (auto outputCol: tables[secondTableIndex].columns) {
					/** Print dividers **/
					cout << "|";
					input = outputCol.colData[j];
					/** Adjust input for character columns **/
					if (outputCol.colType == "char" || outputCol.colType == "varchar") {
						input.erase(0,1);
						input.pop_back();
					}
					cout << input;
				}
				firstInput = true;
				cout << endl;
			}
		}
	}
	/** End Input **/
	return true;
}
/*!
 * \brief Function for the outer join table function
 *
 * This function will select two tables from the internal Table list.
 * When the main parser detects the OUTER JOIN command and a database
 * has been used it will direct to this function. The function will
 * then list all the column information or output the appropriate
 * error messages. This function also leads to the joining functions.
 *
 * \param[in] string firstTable
 *            String corresponding name of the first table to join
 *
 * \return boolean true/false
 *         True or false value depending on state of function. False only sent
 *         if the parser cannot interpret the error and will send true in case
 *         of a processed create command if it is created or not. This function
 *         will additionally detect if the user is selecting all or selecting a
 *         particular column and direct to the correct function.
 */
bool Database::outerJoin(string firstTable) {
	string input; /// String holds input
	string secondTable; /// String holds the second table
	string firstType; /// String holds first compare type
	string secondType; /// String holds second compare type
	int firstTableIndex = -1; /// Int index of table 1
	int secondTableIndex = -1; /// Int index of table 2
	int firstTableTypeIndex = -1; /// Int index of table 1's search type
	int secondTableTypeIndex = -1;/// Int index of table 2's search type
	bool firstInput = true; /// Boolean Stores if first input to print
	bool printedThisRow = false; /// Boolean stores

	/** Get second table name and process **/
	cin >> secondTable;
	cin >> input;
	cin >> input;

	/** Check for correct syntax **/
	if (input != "on")
		return false;
	cin >> input;

	/** Check if proper '.' format **/
	if (input.find('.') == string::npos)
		return false;

	/** Get first table column type **/
	firstType = input.substr(input.find('.') + 1);

	/** Check if proper '=' format **/
	cin >> input;
	if (input != "=")
		return false;

	/** Get second table column type **/
	cin >> input;
	if (input.find('.') == string::npos ) /// Check if proper '.' format
		return false;
	if (input.back() != ';') /// Check if ending in ';'
		return false;
	input.pop_back();
	secondType = input.substr(input.find('.') + 1);

	/** Change names to lower case **/
	transform(firstTable.begin(), firstTable.end(), firstTable.begin(), ::tolower);
	transform(secondTable.begin(), secondTable.end(), secondTable.begin(), ::tolower);

	/** Check if index exists for tables **/
	for (size_t i = 0; i < tables.size(); i++) /// Iterate through all tables
	{
		if (tables[i].lowerName == firstTable) /// Check if name matches table 1
			firstTableIndex = i;
		else if (tables[i].lowerName == secondTable) /// Check if name matches table 2
			secondTableIndex = i;
	}

	/** Check for failure **/
	if (firstTableIndex == -1 || secondTableIndex == -1)
		return false;

	/** Check if index exists for first table **/
	for (size_t i = 0; i < tables[firstTableIndex].columns.size(); i++) {
		if (tables[firstTableIndex].columns[i].colName == firstType)
			firstTableTypeIndex = i;
	}

	/** Check if index exists for first table **/
	for (size_t i = 0; i < tables[secondTableIndex].columns.size(); i++) {
		if (tables[secondTableIndex].columns[i].colName == secondType)
			secondTableTypeIndex = i;
	}

	/** Check for failure **/
	if (firstTableTypeIndex == -1 || secondTableTypeIndex == -1)
		return false;

	/** Print table 1 header **/
	for (auto inputColumn: tables[firstTableIndex].columns) {
		if (firstInput) ///Do not print dividers if first input
			firstInput = false;
		else ///Print dividers
			cout << "|";
		cout << inputColumn.colName << " " << inputColumn.colType;
		/** If char print the column size **/
		if (inputColumn.colType == "char" || inputColumn.colType == "varchar")
			cout << "(" << inputColumn.colSize << ")";
	}

	/** Print table 2 header **/
	for (auto inputColumn: tables[secondTableIndex].columns) {
		/** Print dividers and input **/
		cout << "|" << inputColumn.colName << " " << inputColumn.colType;
		/** If char print the column size **/
		if (inputColumn.colType == "char" || inputColumn.colType == "varchar")
			cout << "(" << inputColumn.colSize << ")";
	}
	firstInput = true;
	cout << endl;

	/** Print joined table **/
	for (size_t i = 0; i < tables[firstTableIndex].rowNum; i++) /// Iterate through table 1
	{
		printedThisRow = false;
		/** Iterate through table 2 **/
		for (size_t j = 0; j < tables[secondTableIndex].rowNum; j++){
			/** Check to see if a matching tuple is found **/
			if (tables[firstTableIndex].columns[firstTableTypeIndex].colData[i] ==
				tables[secondTableIndex].columns[secondTableTypeIndex].colData[j]) {
				printedThisRow = true;
				/** Loop through table 1 columns and print at tuple **/
				for (auto outputCol: tables[firstTableIndex].columns) {
					if (firstInput) ///Do not print dividers if first input
						firstInput = false;
					else ///Print dividers
						cout << "|";
					input = outputCol.colData[i];
					/** Adjust input for character columns **/
					if (outputCol.colType == "char" || outputCol.colType == "varchar") {
						input.erase(0,1);
						input.pop_back();
					}
					cout << input;
				}
				/** Loop through table 2 columns and print at tuple **/
				for (auto outputCol: tables[secondTableIndex].columns) {
					/** Print dividers **/
					cout << "|";
					input = outputCol.colData[j];
					/** Adjust input for character columns **/
					if (outputCol.colType == "char" || outputCol.colType == "varchar") {
						input.erase(0,1);
						input.pop_back();
					}
					cout << input;
				}
				firstInput = true;
				cout << endl;
			}
		}
		/** Print a blank row if no match found **/
		if (printedThisRow == false){
			/** Iterate through first table columns **/
			for (auto outputCol: tables[firstTableIndex].columns) {
				if (firstInput) ///Do not print dividers if first input
					firstInput = false;
				else ///Print dividers
					cout << "|";
				input = outputCol.colData[i];
				/** Adjust input for character columns **/
				if (outputCol.colType == "char" || outputCol.colType == "varchar") {
					input.erase(0,1);
					input.pop_back();
				}
				cout << input;
			}
			for (auto outputCol: tables[secondTableIndex].columns) {
				/** Print dividers **/
				cout << "|";
			}
			firstInput = true;
			cout << endl;
		}
	}
	return true;
}
/*!
 * \brief Function for the alter table function
 *
 * This function will alter a table from the internal Table list.
 * When the main parser detects the ALTER command and a database
 * has been used it will direct to this function. The function will
 * allow the input of a proper column or output the appropriate
 * error messages.
 *
 * \return boolean true/false
 *         True or false value depending on state of function. False only sent
 *         if the parser cannot interpret the error and will send true in case
 *         of a processed create command if it is created or not.
 */
bool Database::alterTable() {
	string input; /// String holding user input
	string tableName; /// String holding the table name

	/** Receive and Check Input From User **/
	cin >> input;
	if (input != "TABLE") /// Unknown command if TABLE not specified
		return false;
	cin >> tableName;
	cin >> input;
	if (input != "ADD") /// Unknown command if ADD not specified
		return false;

	/** Locate internal database use **/
	for (size_t i = 0; i < tables.size(); i++) {
		if (tables[i].tableName == tableName)
			return tables[i].alter();
	}

	/** Output error if location failed **/
	cout << "!Failed to alter table "
		 << tableName
		 << " because it does not exist." << endl;

	return true; /// Return true for end of processing
}
/*!
 * \brief Function for the insert table function
 *
 * This function will insert into a table from the internal Table list.
 * When the main parser detects the INSERT command and a database
 * has been used it will direct to this function. The function will
 * allow the input of a proper column or output the appropriate
 * error messages.
 *
 * \return boolean true/false
 *         True or false value depending on state of function. False only sent
 *         if the parser cannot interpret the error and will send true in case
 *         of a processed create command if it is created or not.
 */
bool Database::insertTable() {
	string input; ///S tring holding user input
	string tableName; /// String holding the table name

	/** Receive and Check Input From User **/
	cin >> input;
	if (input != "into") /// Unknown command if into not specified
		return false;
	cin >> tableName;

	/** Locate internal database use **/
	for (size_t i = 0; i < tables.size(); i++) {
		if (tables[i].tableName == tableName)
			return tables[i].insert();
	}

	/** Output error if location failed **/
	cout << "!Failed to insert into table "
		 << tableName
		 << " because it does not exist." << endl;

	return true; /// Return true for end of processing
}
/*!
 * \brief Function for the update table function
 *
 * This function will update a table from the internal Table list.
 * When the main parser detects the update command and a database
 * has been used it will direct to this function. The function will
 * allow the input of a proper column or output the appropriate
 * error messages.
 *
 * \return boolean true/false
 *         True or false value depending on state of function. False only sent
 *         if the parser cannot interpret the error and will send true in case
 *         of a processed create command if it is created or not.
 */
bool Database::updateTable() {
	string input; /// String holding user input
	string tableName; /// String holding the table name

	/** Receive and Check Input From User **/
	cin >> tableName;

	/** Locate internal database use **/
	for (size_t i = 0; i < tables.size(); i++) {
		if (tables[i].tableName == tableName)
			return tables[i].update();
	}

	/** Output error if location failed **/
	cout << "!Failed to update table "
		 << tableName
		 << " because it does not exist." << endl;

	return true; /// Return true for end of processing
}
/*!
 * \brief Function for the delete from table function
 *
 * This function will delete from a table from the internal Table list.
 * When the main parser detects the delete command and a database
 * has been used it will direct to this function. The function will
 * allow the input of a proper column or output the appropriate
 * error messages.
 *
 * \return boolean true/false
 *         True or false value depending on state of function. False only sent
 *         if the parser cannot interpret the error and will send true in case
 *         of a processed create command if it is created or not.
 */
bool Database::deleteFromTable() {
	string input; /// String holding user input
	string tableName; /// String holding the table name

	/** Receive and Check Input From User **/
	cin >> input;
	if (input != "from") /// Unknown command if TABLE not specified
		return false;
	cin >> tableName;

	/** Convert input to lowercase **/
	transform(tableName.begin(), tableName.end(), tableName.begin(), ::tolower);

	/** Locate internal database use **/
	for (size_t i = 0; i < tables.size(); i++) {
		if (tables[i].lowerName == tableName)
			return tables[i].deleteFrom();
	}

	/** Output error if location failed **/
	cout << "!Failed to delete from table "
		 << tableName
		 << " because it does not exist." << endl;

	return true; /// Return true for end of processing
}
/*!
 * \brief Function for the database's parser
 *
 * This function will use a database from the internal Database list for use
 * in table manipulation. When the main parser detects the USE command and
 * a valid database is given it will direct to this function. From this
 * function the database's individual actions are controlled. If errors occur
 * it will redirect back to the original parser.
 *
 * \return string containing "END" or last used command
 *         When a command is not recognized in the secondary parser it is
 *         brought back to the main parser to interpret. If the secondary
 *         parser encounters an error it will send END signaling to close the
 *         program.
 * \note Directs flow of program to functions or back to main
 */
string Database::dataBaseParser() {
	string input; /// String holding user input
	bool endInput = false; /// Boolean switch if end of input

	/** Main Control Loop **/
	while (!endInput) {
		/** Get Token Input **/
		cin >> input;
		/** Database Creation/Deletion/Use Commands **/
	    if (input == "CREATE" || input == "create") /// CREATE command
	    {
	    	if (!createTable()) /// If failure end loop and fail main parser
	    	{
	    		endInput = true;
	    		input = "END";
	    	}
	    }
	    else if (input == "DROP") /// DROP command
	    {
	    	if (!dropTable()) /// If failure end loop and fail main parser
	    	{
	    		endInput = true;
	    		input = "END";
	    	}
	    }
	    else if (input == "SELECT" || input == "select") /// SELECT command
	    {
	    	if (!selectTable()) /// If failure end loop and fail main parser
	    	{
	    		endInput = true;
	    		input = "END";
	    	}
	    }
	    else if (input == "ALTER") /// ALTER command
	    {
	    	if (!alterTable()) /// If failure end loop and fail main parser
	    	{
	    		endInput = true;
	    		input = "END";
	    	}
	    }
	    else if (input == "insert") /// INSERT command
	    {
	    	if (!insertTable()) /// If failure end loop and fail main parser
	    	{
	    		endInput = true;
	    		input = "END";
	    	}
	    }
	    else if (input == "update") /// UPDATE command
	    {
	    	if (!updateTable()) /// If failure end loop and fail main parser
	    	{
	    		endInput = true;
	    		input = "END";
	    	}
	    }
	    else if (input == "delete") /// DELETE command
	    {
	    	if (!deleteFromTable()) /// If failure end loop and fail main parser
	    	{
	    		endInput = true;
	    		input = "END";
	    	}
	    }
	    else if (input[0] == '-') /// Ignore inputs starting with - and remove line
        {
            cin.ignore();
            getline (cin, input);
        }
	    /** No Acceptable Command **/
	    else
	    	endInput = true;
	}
	return input; /// Return last token or failure string
}
