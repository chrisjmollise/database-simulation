/*!
 * \file Table.cpp
 *
 * \author Christopher Mollise
 * \date 5/6/2020
 *
 * Project 3 for CS457
 *
 * This file controls all each column within a table. This class acts
 * like a struct but can be expanded for additional functionality.
 * The class holds the needed information to form a table.
 *
 */

#include "../include/Column.h"

///std name space for general use
using namespace std;

/*!
 * \brief Constructor of the column class
 *
 * This function will construct the Column class by reading its
 * setting its individual attributes based on input.
 *
 * \param[in] string newColName
 *            String corresponding to the column name
 * \param[in] string newColType
 *            String corresponding to the column type
 * \param[in] int newColSize
 *            Int corresponding to the column size
 */
Column::Column(string newColName, string newColType, int newColSize) {
    colName = newColName;
    colType = newColType;
    colSize = newColSize;
}
/*!
 * \brief Deconstructor of the Column class
 *
 * This function will deconstruct the Column class. This currently has no
 * effect on the class.
 *
 */
Column::~Column() {
    //dtor
}
