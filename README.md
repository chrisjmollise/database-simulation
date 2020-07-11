# Programming Assignment 3: Table Joins

This project is the tertiary step of a database simulator for CS457. This program allows a database user to manage 
the metadata of their relational data as well as being able to manipulate the data within the table. The main 
functionalities of this project are to simulate a relational database through the use of various linux directories 
representing databases and their files representing tables. Through this simulation it is possible to create and 
delete databases as well as add, delete, select, and alter tables within these databases. 

Additionally, the program can now insert, modify, query, and delete information on the table itself.

In this final version, multiple tables can be selected in order to inner or outer join them. 

## Implementation

This c++ project makes use of several objects in order to keep track of information within the database efficiently.
The project contains three main classes with a class for databases, tables, and columns.

The project utilizes a list of databases acting as virtual representations of physical file directories. 
The main program file "main.cpp " manages the database list, constructing it at the beginning of each program 
operation and controlling the main parser for the program. This file also manages functions such as the creation 
and deletion of directories in the list.

The database object controls the individual database and the tables within them. Each database object contains a
list of tables and has the necessary functionality to create and delete tables from the database as well as alter
or query a table within the database. This functionality is done through a secondary parser that handles functions 
used directly on a database. 

The table object controls the information within an individual table. This is implemented through a list of column 
objects that acts as a storage mechanism for any information a column could contain. This includes metadata such as 
the name, type, and size of data within a column as well as the physical data itself.  The table additionally 
implements functionality such as printing a query or adding new columns to a table.

The last iteration of the program expands on the table object allowing the ability to insert and modify information as
well as query and delete information based on user input. These functions are accessed via the parser in the table 
object and can be adjusted based on user input. This means that functions such as delete, select, and alter will
take user input through column names and conditional statements. These functions are minimal but can be expanded if need
be for a future iteration of this program.

Finally, this iteration of the program implements the ability to join tables using the '=' condition. This allows two
tables with equivalent information in a column to be compared. This is done via the main parser for the Select command
and splitting into two functions for the inner and outer joining of tables. This program allows for an inner join only
showing matches and an outer join showing matches and non-matched tuples from the first table. 


### Prerequisites

This program was built using the Cross GCC toolchain and and is build with C++14 (--std=c++1y) and makes use of the 
libstdc+ library. This project runs on UNR's ECC computers on Ubuntu and will have the prerequisties needed to run 
the program with the given files. 

### Installing

To install this project, download the given compressed file and open on an ECC computer. The program files are 
included in the main directory or in secondary include / src directories. The Debug directory contains the main 
executable project and the nessecary make files for the project. The project can be made using the MAKE command in
the debug directory. 

Example of installation from :~/cmollise_pa3/CS457-2 (The unzipped projects files)

1. :~/cmollise_pa3/CS457-3$ cd Debug
2. :~/cmollise_pa3/CS457-3/Debug$ make

Testing:

3. :~/cmollise_pa3/CS457-3/Debug$ ./CS457-3 <PA3_test.sql



## Testing

The program can be utilized through both traditional keyboard inputs as well as using the standard input key "<". 
All actions to the database are shown through messages on the command terminal. As the program runs the database 
is physically created in the Databases directory created in the Debug directory. With in this directory will be the 
directories for each individual database within the text files for each table. 

To run the test go to the Debug directory to be able to launch the project. From this directory the program can 
be started using
	# ./CS457-3 <enter>
This will allow a user to input commands as show in the expected input. Additionally the program can be ran using 
the standard input "<"
	# ./CS457-3 <PA3_test.sql

## Expected Input

--CS457 PA3

--Construct the database and table (0 points; expected to work from PA1)
CREATE DATABASE CS457_PA3;
USE CS457_PA3;
create table Employee(id int, name varchar(10));
create table Sales(employeeID int, productID int);

--Insert new data (0 points; expected to work from PA2)
insert into Employee values(1,'Joe');
insert into Employee values(2,'Jack');
insert into Employee values(3,'Gill');
insert into Sales values(1,344);
insert into Sales values(1,355);
insert into Sales values(2,544);

-- The following will miss Gill (2 points)
select * 
from Employee E, Sales S 
where E.id = S.employeeID;

-- This is the same as above but with a different syntax (3 points)
select * 
from Employee E inner join Sales S 
on E.id = S.employeeID;

-- The following will include Gill (5 points)
select * 
from Employee E left outer join Sales S 
on E.id = S.employeeID;

.exit

## Expected Output

Database CS457_PA3 created.
Using database CS457_PA3.
Table Employee created.
Table Sales created.
1 new record inserted.
1 new record inserted.
1 new record inserted.
1 new record inserted.
1 new record inserted.
1 new record inserted.
id int|name varchar(10)|employeeID int|productID int
1|Joe|1|344
1|Joe|1|355
2|Jack|2|544
id int|name varchar(10)|employeeID int|productID int
1|Joe|1|344
1|Joe|1|355
2|Jack|2|544
id int|name varchar(10)|employeeID int|productID int
1|Joe|1|344
1|Joe|1|355
2|Jack|2|544
3|Gill||
All done.

## Built With

* Linux GCC Toolchain
* C++14
* Eclipse IDE


## Authors

* **Christopher Mollise** - 5/6/20 - 



