#ifndef _DATABASE_H
#define _DATABASE_H

// This is used to get rid of warnings for using older string functions
#define _CRT_SECURE_NO_DEPRECATE

// Include our basic functions
#include <iostream>
#include <string>
using namespace std;


// In order to use the ADO API for working with databases we need to include
// the msado15.dll in our system.  We use COM to use the DLL's functions.
// The rename() function just renames the EOF definitions to ADOEOF when importing
// the dll file.
#import "C:\Program files\Common Files\System\Ado\msado15.dll" rename("EOF", "ADOEOF")


// This is our database class
class CMyDatabase
{
public:

	// Our constructor and deconstructor
	CMyDatabase();
	~CMyDatabase();

	// This tells us if we are connected or not
	bool IsConnected()	{ return m_connection; }

	// This returns the record count in the table
	int GetRecordCount(string strTable);

	// This connects to an Access Database (.mdb)
	void Connect(string strDatabaseFile);

	// This connects to an SQL server
	void Connect(string strAddress, string strDB, string strUser, string strPassword);

	// This creates a table in the database with column values
	void CreateTable(string strTable, string strValues);

	// This selects a table
	void SelectTable(string strTable);

	// This deletes a table
	void Delete(string strTable, string strWhere = "");

	// This inserts a record into a table
	void InsertRecord(string strTable, string strValues, string strColumns = "");

	// This updates a record's values
	void UpdateRecord(string strTable, string strValues, string strWhere);

	// This deletes a table
	void DeleteTable(string strTable);

	// This runs a SQL command
	int RunSQL(char *szSQL);

	// These return a string from a desired row and column
	string GetString(string strTable, string strWhere, string strField);
	string GetString(string strTable, int index, string strField);

	// These return an integer from a desired row and column
	int GetInt(string strTable, string strWhere, string strField);
	int GetInt(string strTable, int index, string strField);

	// This closes a record set
	void CloseRecordSet();

	// This releases everything
	void Shutdown();

private:

	// These are our connection and recordset pointers
	ADODB::_ConnectionPtr m_connection;
	ADODB::_RecordsetPtr m_recordset;
};


#endif


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// In this file we create the class for our CMyDatabase class.  In
// order to use the ADO functions for messing with databases we need
// to import the msado15.dll. 
//
//
// © 2000-2006 GameTutorials 