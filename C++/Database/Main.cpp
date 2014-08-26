//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Database										 //
//																		 //
//		$Description:	Reading and writing to an SQL or Access database //
//																		 //
//***********************************************************************//

#include "Database.h"


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// In this tutorial we will be going over reading from databases.  This includes
// reading from a Microsoft Access .MDB database, as well as connecting to an
// SQL server from an IP address.  We created a CMyDatabase class that handles our 
// initialization and working with our data.
//
// * What is a database? *
//
// A database is a way to store data in a secure and/or organized way.  Some
// database types include Microsoft's Access and SQL, as well as the free MySQL.
//
// * Why use a database? *
//
// You could create a file and save and read data from it like normal, but when
// using a database you can secure the data with a password, as well as use the
// SQL language to query and store the data.  Another nice thing about using
// databases is that you can connect to them through an IP address easily.  All
// you need the IP address, the database name, and the login and password.
//
// * What is SQL? *
//
// SQL stands for Structured Query Language, and it is pronounced "sequel".  SQL 
// is a language to manipulate a database, as well as extract data from it.  The
// language was developed by IBM in the 1970s.
//
// * What does this tutorial do? *
//
// What this tutorial will show is how to connect to a database (.mdb Access file),
// the read and print its data, add data, then delete the data that was added.
// In our database we have a bunch of player data.  Each player has a name, an
// x and y position, the current level and health points.
//
// * How do I edit the database normally? *
//
// With this tutorial there is a database file called "GTDatabase.mdb".  You
// can open this file in Microsoft Access.  Generally, if you don't want to
// have many users accessing the database at the same time you can use an
// Access database; however, if you will be having multiple users using the
// database at the same time you will want to use something like Microsoft
// SQL Server 2000 or MySQL.
//
// Let's get to the code.  This file handles creating and using our CMyDatabase
// class, where Database.cpp and Database.h define our class functions.
//


// Let's create a global database object to load our database
CMyDatabase g_AccessDatabase;


// This function takes the table of the database we want to open 
// and the name of the player that we want to print.
void PrintPlayerData(string strTable, string strName)
{
	// In a database we have things called "tables".  We store our
	// data for each player in our "Players" table.  We tell our database
	// using the SQL language what table to open and what row of data we
	// want to access.  When we create a table we specify what colums
	// there should be for each row, and what type of data it should store.
	// For instance, in our database we have a column for the player's name,
	// health, position and level.  Each column has a type, like int or
	// string.
	//
	// So, now we want to get the right player's data, so we create a SQL
	// statement to send to our database.  In SQL there is a "WHERE" command
	// that is used to tell the database which data we want.  Below we create
	// a string to pass into our GetString() and GetInt() functions.  If we
	// passed in "Todd" as the strName, the string would be: "Player='Todd';
	// "Player" is the column name for the player's name.  So, the SQL statement
	// would read in words, "Access a string (or int) WHERE the player's name
	// is Todd."
	string strWhere = "Player='" + strName + "'";

	// Now we will extract the player's data by passing in the table, the
	// "where" statement, and the column.  If the data is stored as a string
	// we use GetString(), or we will use GetInt() for numbers.

	// Extract the player's name
	string strPlayer = g_AccessDatabase.GetString(strTable, strWhere, "Player");

	// Extract the player's health points
	int hp = g_AccessDatabase.GetInt(strTable, strWhere, "HP");

	// Extract the player's level, which is stored as a string (i.e. a level file).
	string strLevel = g_AccessDatabase.GetString(strTable, strWhere, "Level");

	// Extract the x and y position
	int positionX = g_AccessDatabase.GetInt(strTable, strWhere, "PositionX");
	int positionY = g_AccessDatabase.GetInt(strTable, strWhere, "PositionY");

	// Print the data to the screen
	cout << "Player: " << strPlayer << endl
		 << "HP: "     << hp		<< endl
		 << "Level: "  << strLevel  << endl
		 << "(X,Y): (" << positionX << "," << positionY << ")" << endl;
}


// This is our main() function for the console application
int main()
{
	// The first thing we want to do is connect to our database.  Since
	// we are using an Access database we will use the first Connect()
	// function where we just pass in the database file.  If we are connecting
	// to a SQL database with an IP address we would use the second Connect()
	// function.  Check below in the QUICK NOTES section for an example of this.
	g_AccessDatabase.Connect("GTDatabase.mdb");

	// Since we use the name of the database table a lot, let's store it.
	string strTable = "Players";

	// This next line returns the record count in the desired table.  The
	// record count refers to the rows in the table.
	int recordCount = g_AccessDatabase.GetRecordCount(strTable);

	// Print out our record count.
	cout << "Current Records:" << recordCount << endl;
	cout << endl << endl;

	// Now let's extract Todd's data and print it out to the screen
	PrintPlayerData(strTable, "Todd");
	cout << endl << endl;

	// Print out Rhett's data
	PrintPlayerData(strTable, "Rhett");
	cout << endl << endl;

	// We can also update a player's data.  We pass in the table, the
	// column and its new value, and finally a WHERE statement that tells
	// us that we want to change the health of Todd.
	g_AccessDatabase.UpdateRecord(strTable, "HP='13'", "Player='Todd'");

	// Now print out Todd's new data
	PrintPlayerData(strTable, "Todd");
	cout << endl << endl;

	// Next, we will show how to insert a new record (or row) into the table.
	// We create a string that stores the name, health, level and (x,y) position.
	char szData[512] = {0};
	sprintf(szData, "'%s', %d, '%s', %d, %d", "Seth", 1000, "Level1.gtl", 100, 200);
	
	// Now insert the data into our table.  The record will now be at the end.
	g_AccessDatabase.InsertRecord(strTable, szData);

	// Now let's get and print the record count with our new record in the database.
	recordCount = g_AccessDatabase.GetRecordCount(strTable);
	cout << "Current Records:" << recordCount << endl;
	cout << endl << endl;

	// Print the new player's data
	PrintPlayerData(strTable, "Seth");
	cout << endl << endl;
	
	// Next, let's restore the database to its original data by deleting Seth.
	// We pass in the table and the WHERE statement (where Player equals Seth).
	g_AccessDatabase.Delete(strTable, "Player='Seth'");

	// Finally, get the record count and print it to the screen.
	recordCount = g_AccessDatabase.GetRecordCount(strTable);
	cout << "Current Records:" << recordCount << endl;

	// Pause so the console window stays open
	system("pause");

	// Return a success
	return 1;
}



////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// Basically, to open a database we use the Connect() function, then
// we can call functions to access data, update data, add and delete
// record sets, as well as get the record count.
//
// Once again, a record is a row in the database that holds the data
// for each column in the database.
//
// * Connecting to a SQL server *
//
// In this tutorial we simple connect to an Access database since it's
// easy to demonstrate, but if you need to connect to a database
// through an IP address on a web server or your own computer, the
// following code will do it:
//
// g_SQLDatabase.Connect(strSQLAddress,  strSQLDB,   strSQLLogin,  strSQLPassword);
// 
// An example might be: (none of this data is from an actual server)
// 
// g_SQLDatabase.Connect("44.173.15.79", "GTData", "digiben", "password123");
//
// After we connect to an SQL server, the other functions will work the
// same.  Now check out Database.cpp and then Database.h to see the
// underworkings of the database code.
//
//
// © 2000-2006 GameTutorials 