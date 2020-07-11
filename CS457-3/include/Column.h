/*!
 * \file Column.h
 *
 * \author Christopher Mollise
 * \date 5/6/2020
 *
 * Project 3 for CS457
 *
 * Header for Column Class
 *
 */

#ifndef COLUMN_H
#define COLUMN_H

#include <string>
#include <vector>

/*!
 * \class Column
 *
 * \brief This class acts as a virtual implementation of a column
 *
 * This class holds the information for a column within a table.
 * This virtual column will allow for easier data manipulation as
 * the data will be already loaded in the program.
 *
 * \author Christopher Mollise
 * \date 5/6/20
 */
class Column {
	public:
		/** Default Constructor **/
		Column(std::string newColName, std::string newColType, int newColSize);
		virtual ~Column();///Default deconstructor

		std::vector<std::string> colData; ///Vector of strings for data
		std::string colName; ///String of the column name
		std::string colType; ///String of the column type
		int colSize; ///Int of the column size
};

#endif // COLUMN_H
