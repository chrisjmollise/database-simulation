/*!
 * \file main.cpp
 *
 * \author Christopher Mollise
 * \date 5/6/2020
 *
 * Project 3 for CS457
 * This program is a relational database simulator. The main function of
 * this project is to implement simple SQLite commands without utilizing
 * SQLite. This is done through the use of several objects containing
 * relevant database information such as the database itself, the tables
 * within the database, and individual columns in the database. Currently,
 * the simulator can create/delete databases, create/delete tables, and
 * add to or select from a table.
 *
 * It can also insert into the table,
 * update the table with new information, delete information from the table
 * based on conditions, and select from the table based on conditions.
 *
 * Functionality now exists in the program to simulating joining with both
 * an inner join and outer join.
 *
 * This file is the main line of execution. From this file the database is
 * read into the program to act as a starting point for other manipulations.
 * This file also contains the main parser and handles the main database
 * commands. Commands that are not executed by this parse are sent to
 * individual parsers in the Database class and beyond. If a command cannot
 * be executed from any parser it will return to the main parser function to
 * state an error and end the program.
 *
 */

#include <experimental/filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "include/Database.h"

/// Std name space for general use
using namespace std;

/// Name space for use for shortening the file system name space
/// Not using whole name space as it is not needed
namespace fs = std::experimental::filesystem;

 /*!
 * \brief Initial configuration system for database
 *
 * This function acts as the base for updating the database objects on load.
 * When the program first executes it will call this function, constructing
 * the database based on the physical database files. This is necessary
 * to create the one to one relation between the programs simulated database
 * and the actual database. Will construct the individual databases from
 * the directory and then add the name of the database to a public variable.
 *
 * \param[out] vector<Database> &dataBases
 *             Vector of the Database class used for containing all data
 * \note Makes use of the default class constructors of Database.
 */
void updateDataBase(vector<Database> &dataBases) {
	string name; ///String containing database name

	/** Directory Reader **/
	for(auto &p: fs::directory_iterator("Databases")){
        /// Push new database to vector for each directory found
		dataBases.push_back(Database(p.path()));
        /// Get the name of the directory from the path and add to class
		name = p.path().string();
		name = name.substr(name.find('/') + 1);
		dataBases.back().dataBaseName = name;
	}
}
/*!
 * \brief Function for the create database function
 *
 * This function will add a database to the internal Database list and then
 * replicate this database on a file level. When the main parser detects the
 * CREATE command and no database has been used it will direct to this
 * function. The function will then create a directory based on the name and
 * accept or deny it from the database if the directory does or does not
 * exist. It will then output the appropriate error messages.
 *
 * \param[out] vector<Database> &dataBases
 *             Vector of the Database class used for containing all data
 * \return  boolean true/false
 *          True or false value depending on state of function. False only
 *          sent if the parser cannot interpret the error and will send true
 *          in case of a processed create command if it is created or not.
 */
bool createDataBase(vector<Database> &dataBases) {
    string input; /// String containing input from user
    fs::path dataPath = "Databases/"; /// Directory path for created database

    /** Receive and Check Input From User **/
    cin >> input;
    if (input != "DATABASE") /// Unknown command if DATABASE not specified
        return false;
    cin >> input;
    if (input.back() != ';') /// Unknown command if no ";" at end of function
        return false;

    /** Process path for creating directory **/
    input.pop_back();
    dataPath += input;

    /** Attempt to create directory at path **/
    if(!fs::create_directory(dataPath)) /// Create and output error if fails
    {
        cout << "!Failed to create database "
             << input
             << " because it already exists." << endl;
    }
    else /// Output message if success and add database to internal list
    {
    	dataBases.push_back(Database(dataPath));
    	dataBases.back().dataBaseName = input;
        cout << "Database "
             << input
             << " created." << endl;
    }
    return true; /// Return true for end of processing
}
/*!
 * \brief Function for the drop database function
 *
 * This function will drop a database from the internal Database list and then
 * replicate this deletion on a file level. When the main parser detects the
 * DROP command and no database has been used it will direct to this
 * function. The function will then drop a directory based on the name and
 * drop or deny it from the database if the directory does or does not
 * exist. It will then output the appropriate error messages.
 *
 * \param[out] vector<Database> &dataBases
 *             Vector of the Database class used for containing all data
 * \return boolean true/false
 *         True or false value depending on state of function. False only sent
 *         if the parser cannot interpret the error and will send true in case
 *         of a processed create command if it is created or not.
 */
bool dropDataBase(vector<Database> &dataBases) {
	string input; /// String containing input from user
    fs::path dataPath = "Databases/"; /// Directory path for deleted database

    /** Receive and Check Input From User **/
	cin >> input;
	if (input != "DATABASE") /// Unknown command if DATABASE not specified
        return false;
    cin >> input;
    if (input.back() != ';') /// Unknown command if no ";" at end of function
        return false;

    /** Process path for deleting directory **/
    input.pop_back();
    dataPath += input;

    /** Attempt to delete directory at path **/
    if(!fs::remove_all(dataPath)) /// Delete and output error if fails
    {
        cout << "!Failed to delete "
             << input
             << " because it does not exist." << endl;
    }
    else /// Output message if success and add database to internal list
    {
        /** Locate internal database to also remove **/
        for (size_t i = 0; i < dataBases.size(); i++){
            if (dataBases[i].dataBaseName == input)
            	dataBases.erase(dataBases.begin() + i);
        }

        cout << "Database "
             << input
             << " deleted." << endl;
    }
    return true; /// Return true for end of processing
}
/*!
 * \brief Function for the use database command
 *
 * This function will use a database from the internal Database list for use
 * in table manipulation. When the main parser detects the USE command
 * it will direct to this function. The function will use the database if it
 * exists or give an error message if it does not. It will then call the
 * parser for the individual database for use of their parser.
 *
 * \param[out] vector<Database> &dataBases
 *             Vector of the Database class used for containing all data
 * \return string containing "END" or last used command
 *         When a command is not recognized in the secondary parser it is
 *         brought back to the main parser to interpret. If the secondary
 *         parser encounters an error it will send END signaling to close the
 *         program.
 * \note Directs flow of program to the parser in the Database class.
 */
string useDataBase(vector<Database> &dataBases) {
	string input; /// String containing input from user

	/** Receive and Check Input From User **/
	cin >> input;
	if (input.back() != ';')
		return "END";

    /** Process input before selecting internal database **/
	input.pop_back();

	/** Locate internal database use **/
	for (auto inputDataBase: dataBases){
		if (inputDataBase.dataBaseName == input) {
			cout << "Using database "
			     << input
				 << '.' << endl;
             /** Return function call to the database parser command **/
			return inputDataBase.dataBaseParser();
		}
	}

	/** Output error if location failed **/
	cout << "!Failed to use database "
	     << input
	     << " because it does not exist." << endl;

	return "END"; ///Return END to signal ending program
}
/*!
 * \brief Function for the use database command
 *
 * This function will use a database from the internal Database list for use
 * in table manipulation. When the main parser detects the USE command
 * it will direct to this function. The function will use the database if it
 * exists or give an error message if it does not. It will then call the
 * parser for the individual database for use of their parser.
 *
 * \param[out] vector<Database> &dataBases
 *             Vector of the Database class used for containing all data
 * \note Controls main flow of project with while loop controlling all parsing
 */
void mainParser(vector<Database> &dataBases) {
    string input; /// String containing input from user
    bool endInput = false; /// Boolean switch to end program
    bool returnedInput = false; /// Boolean switch to use secondary parse input

    /** Main Control Loop **/
    while (!endInput) {
        if (returnedInput) /// Check if input was passed from secondary parser
            returnedInput = false;
        else /// Receive an input token from user
            cin >> input;
        /** Exit and Ignore Commands **/
        if (input[0] == '-') /// Ignore inputs starting with - and remove line
        {
            cin.ignore();
            getline (cin, input);
        }
        else if (input == ".EXIT" || input == ".exit") /// EXIT command
        {
        	cout << "All done." << endl;
        	return;
        }

        /** Database Creation/Deletion/Use Commands **/
        else if (input == "CREATE") /// CREATE command
        {
            if (!createDataBase(dataBases)) /// Call create function for failure
                endInput = true;
        }
        else if (input == "DROP") /// DROP command
        {
            if (!dropDataBase(dataBases)) /// Call drop function for failure
            	endInput = true;
        }
        else if (input == "USE") /// USE command
        {
        	input = useDataBase(dataBases); /// Call use function for input
        	if (input == "END") /// Check if secondary parser failed
        		endInput = true;
        	else /// Tell main parser to not receive a new input for secondary
        		returnedInput = true;
        }

        /** No Acceptable Command **/
        else
            endInput = true;
    }
    /** Send Unknown Error if parser fails **/
    cout << "!Unknown Command." << endl;
}
/*!
 * \brief Function for the main program execution
 *
 * This function will initialize the project. First the function will create
 * the list containing all databases and populate it. It will also create
 * the "Databases" folder that contains all database info if it does not exist.
 * The program will then call the main parser and run the project.
 *
 * \note Main function
 */
int main() {
	vector <Database> dataBases; /// List object containing all databases
	/** Create Directory for project use **/
	fs::create_directory("Databases");
	/** Update the virtual database to match the physical database **/
	updateDataBase(dataBases);
	/** Call the main parser **/
    mainParser(dataBases);
    /** End the Program **/
    return 0;
}
