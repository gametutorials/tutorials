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
// Before going over this file, check out Main.cpp to get the overview of the
// tutorial.  This file has our definitions for our CMyDatabase class.
//
// The API we will be using to connect to our database is called ADO.  This is 
// the API that you will find used quite often.
//
// Let's go over the code that you will need to use when working with databases.
//


// The constructor of the database class
CMyDatabase::CMyDatabase()
{
	// Set our member variables to NULL
	m_connection = NULL;
	m_recordset = NULL;
}


// The deconstructor for our class
CMyDatabase::~CMyDatabase()
{
	// Make sure the connection is closed and data freed
	Shutdown();
}


// This function returns our record count in the desired table
int CMyDatabase::GetRecordCount(string strTable)
{
	// Check if our database is connected, otherwise return.
	if(!IsConnected()) 
		return -1;

	// Now we need to create the SQL statement to work with the database.
	// The SELECT command selects columns from a table and returns the row
	// data for those columns.  We use the '*' character to say that we want
	// ALL the columns.  If you want only specific columns you can use commas.
	char szSQLStatement[255] = {0};
	sprintf(szSQLStatement, "SELECT * FROM %s", strTable.c_str());

	// Here we input our SQL statement into the Open() function of our _RecordsetPtr
	// object that was created when we initially connected to the database.  This
	// function also takes an interface pointer for COM, the direction we will
	// traverse the recordset list (forward), permission settings (lock type), and
	// any extra options (adCmdText, which says we are passing in a string for a
	// SQL command).
	m_recordset->Open(szSQLStatement, m_connection.GetInterfacePtr(),
									  ADODB::adOpenForwardOnly, ADODB::adLockReadOnly, ADODB::adCmdText);

	// Before closing the recordset we need to grab the record count
	int rowCount = m_recordset->RecordCount;
	CloseRecordSet();

	// Return the row count
	return rowCount;
}


// Connect to a Microsoft Access database file (.mdb)
void CMyDatabase::Connect(string strDatabaseFile)
{
	// If we are connected already, let's destroy our connection.
	if(m_connection) 
		Shutdown();

	// Since we are using COM to connect to the database we need to
	// call CoInitialize().  We also create a HRESULT to store results.
	HRESULT hResult;
	CoInitialize(NULL);

	// Now we want to use the "try" keyword to see if we can connect
	try
	{
		// Here we pass in the ADO Connection GUID to CreateInstance()
		hResult = m_connection.CreateInstance(__uuidof(ADODB::Connection));

		// If we were able to create an instance of the ADO COM object
		// for a connection we will throw a COM error, otherwise let's 
		// go and connect.
		if (FAILED(hResult))
		{
			throw _com_error(hResult);
		}

		// If we get here, we were able to create an ADO instance, but
		// we also need to create an instance for a recordset object.
		hResult = m_recordset.CreateInstance(__uuidof(ADODB::Recordset));

		// If we weren't able to create a recordset instance, throw an error.
		if (FAILED(hResult))
		{
			throw _com_error(hResult);
		}

		// To connect to the database we need to use the following string to tell COM that
		// we will be using Microsoft's Jet Engine as the "Providor" to connect to our database.
		// If you are connecting to a database that was created by Access 2.0 or above you
		// want to use the 4.0 version of the Jet Engine.  For more information about this
		// engine check out http://support.microsoft.com/?KBID=300216.  For our "Data
		// Source" we use our database file.
		char szConnection[255] = {0};
		sprintf(szConnection, "Provider=Microsoft.Jet.OLEDB.4.0; Data Source=%s", strDatabaseFile.c_str());

		// Now we want to set our cursor location to adUseClient, which gives us a accurate
		// count when extracting our record count.
		m_connection->CursorLocation = ADODB::adUseClient;

		// To convert our char string to a bstr we use the _bstr_t() function.
		_bstr_t strConnection = _bstr_t(szConnection);

		// Finally, open a connection to the database by passing in the connection string,
		// userID, password, and options for asynchronous or synchronous connection.  We use
		// adConnectUnspecified for a synchronous connection, or you can use adAsyncConnect.
		// We pass in blank bstr's for the user ID and password since we don't have one.
		m_connection->Open(strConnection, _bstr_t (""), _bstr_t (""), ADODB::adConnectUnspecified);
	}

	// If we received any errors during our "try" scope, post an error message
	catch(_com_error &e)
    {
		MessageBox(NULL, static_cast<char *>(e.Description()), "Error", MB_OK);
    }

    catch(...)
    {
		MessageBox(NULL, "Unhandled Exception", "Error", MB_OK);
    };
}


// This function connects to an SQL server with the ip, database name, user and password.
void CMyDatabase::Connect(string strAddress, string strDB, string strUser, string strPassword)
{
	// This function is just like the other connection function, except our
	// connection string is different.  Check below for the connection string.

	// If we are connected already, let's destroy our connection.
	if(m_connection) 
		Shutdown();

	// Since we are using COM to connect to the database we need to
	// call CoInitialize().  We also create a HRESULT to store results.
	HRESULT hResult;
	CoInitialize(NULL);

	// Now we want to use the "try" keyword to see if we can connect
	try
	{
		// Here we pass in the ADO Connection GUID to CreateInstance()
		hResult = m_connection.CreateInstance(__uuidof(ADODB::Connection));

		// If we were able to create an instance of the ADO COM object
		// for a connection we will throw a COM error, otherwise let's 
		// go and connect.
		if (FAILED(hResult))
		{
			throw _com_error(hResult);
		}

		// If we get here, we were able to create an ADO instance, but
		// we also need to create an instance for a recordset object.
		hResult = m_recordset.CreateInstance(__uuidof(ADODB::Recordset));

		// If we weren't able to create a recordset instance, throw an error.
		if (FAILED(hResult))
		{
			throw _com_error(hResult);
		}

		// Since we are not just connecting to an Access database file, we need 
		// to create a connection string to connect to the database through an IP
		// address.  Our "Provider" is "sqloledb", and our "Data Source" is the
		// IP address to the database server.  In our connection string we also
		// need to give the database name and the user name and password.
		char szAddress[255] = {0};
		char szConnection[255] = {0};
		sprintf(szAddress,	  "Provider=sqloledb; Data Source= %s;", strAddress.c_str());
		sprintf(szConnection, "%s Initial Catalog=%s;User Id=%s;Password=%s;", szAddress, strDB.c_str(), strUser.c_str(), strPassword.c_str());

		// Now we want to set our cursor location to adUseClient, which gives us a accurate
		// count when extracting our record count.
		m_connection->CursorLocation = ADODB::adUseClient;

		// To convert our char string to a bstr we use the _bstr_t() function.
		_bstr_t strConnection = _bstr_t(szConnection);

		// Finally, open a connection to the database by passing in the connection string,
		// userID, password, and options for asynchronous or synchronous connection.  We use
		// adConnectUnspecified for a synchronous connection, or you can use adAsyncConnect.
		// We don't need to pass in a user ID or password because it's in the connection string.
		m_connection->Open(strConnection, _bstr_t (""), _bstr_t (""), ADODB::adConnectUnspecified);
	}

	// If we received any errors during our "try" scope, post an error message
	catch(_com_error &e)
    {
		MessageBox(NULL, static_cast<char *>(e.Description()), "Error", MB_OK);
    }

    catch(...)
    {
		MessageBox(NULL, "Unhandled Exception", "Error", MB_OK);
    };
}


// This function runs an SQL statement through a char string
int CMyDatabase::RunSQL(char *szSQL)
{
	int result = 0;

	// Here we "try" to run our SQL statement using Execute()
	try
	{
		result = (int)m_connection->Execute(szSQL, NULL, NULL);
	}
	
	// If there was an error running the statement display the error messsage
	catch(_com_error &e)
    {
		// By casting the com error object to a string we can get the full
		// description of the error.
		MessageBox(NULL, static_cast<char *>(e.Description()), "Error", MB_OK);
    }

    catch(...)
    {
		MessageBox(NULL, "Unhandled Exception", "Error", MB_OK);
    };

	// Return the result of the SQL statement
	return result;
}


// This creates a table in the database with a table name and string of column values
void CMyDatabase::CreateTable(string strTable, string strValues)
{
	// Use the CREATE TABLE keyword to create a table and columns
	char szSQLStatement[255] = {0};
	sprintf(szSQLStatement, "CREATE TABLE %s (%s)", strTable.c_str(), strValues.c_str());

	// Run the SQL statement
	RunSQL(szSQLStatement);
}


// This selects a specific table
void CMyDatabase::SelectTable(string strTable)
{
	// Select all the columns from the desired table
	char szSQLStatement[255] = {0};
	sprintf(szSQLStatement, "SELECT * FROM %s", strTable.c_str());

	// Run the SQL statement
	RunSQL(szSQLStatement);
}


// This deletes data from the database from the table name and the WHERE statement.
void CMyDatabase::Delete(string strTable, string strWhere)
{
	char szSQLStatement[255] = {0};

	// If there is a WHERE statement we will only delete rows according to the
	// WHERE statement, otherwise delete all the rows in the table.
	if(strWhere.size() > 0)
		sprintf(szSQLStatement, "DELETE FROM %s WHERE %s", strTable.c_str(), strWhere.c_str());
	else
		sprintf(szSQLStatement, "DELETE * FROM %s", strTable.c_str());

	// Run the SQL statement
	RunSQL(szSQLStatement);
}


// Insert a new record in a table from the table name, values and columns.
void CMyDatabase::InsertRecord(string strTable, string strValues, string strColumns)
{
	char szSQLStatement[255] = {0};

	// If there are columns specified, add the values only to those columns,
	// otherwise add the values for every column.
	if(strColumns.size() > 0)
		sprintf(szSQLStatement, "INSERT INTO %s (%s) VALUES (%s)", strTable.c_str(), strColumns.c_str(), strValues.c_str());
	else
		sprintf(szSQLStatement, "INSERT INTO %s VALUES (%s)", strTable.c_str(), strValues.c_str());

	// Run the SQL statement
	RunSQL(szSQLStatement);
}


// This updates a record's data with the table name, values and a WHERE statement
void CMyDatabase::UpdateRecord(string strTable, string strValues, string strWhere)
{
	// Use the UPDATE keyword to SET the values in the specified rows through a WHERE.
	char szSQLStatement[255] = {0};
	sprintf(szSQLStatement, "UPDATE %s SET %s WHERE %s", strTable.c_str(), strValues.c_str(), strWhere.c_str());

	// Run the SQL statement
	RunSQL(szSQLStatement);
}


// This returns a string from a row's column
string CMyDatabase::GetString(string strTable, string strWhere, string strField)
{
	// Create the SQL statement to SELECT all the columns in the row FROM the
	// table WHERE we specify.  To increase the speed we could just SELECT the
	// strField instead of all of the columns (instead of *).
	char szSQLStatement[999] = {0};
	sprintf(szSQLStatement, "SELECT * FROM %s WHERE %s", strTable.c_str(), strWhere.c_str());

	// Open the database and run the SQL statement
	m_recordset->Open(szSQLStatement, m_connection.GetInterfacePtr(),
									  ADODB::adOpenForwardOnly, ADODB::adLockReadOnly, ADODB::adCmdText);
	
	// Now we create a _variant_t variable that stores different types of data.
	_variant_t var;

	// Using our recordset pointer, we access the GetValue() function by
	// passing in the column name into GetItem().  GetValue returns an
	// ambiguous _variant_t that we can cast to the appropriate type.
	var = m_recordset->Fields->GetItem(strField.c_str())->GetValue();

	// Set our final string value to blank first off.
	string strValue = "";

	// If we couldn't find the string we don't want the program to crash so we
	// check if it is null, or "VT_NULL".  Remember, null means "nothing".
	if(var.vt != VT_NULL)
	{	
		// Since we want a string, we know that we need to convert our var
		// variable to a bstr, and then casted to a char string.
		strValue = static_cast<char *>(_bstr_t(var.bstrVal));
	}
	else
		strValue = "";	// Make our string blank because we didn't find our string

	// Close the recordset
	CloseRecordSet();

	// Return our string we got from the database
	return strValue;
}


int CMyDatabase::GetInt(string strTable, string strWhere, string strField)
{
	// Create the SQL statement to SELECT all the columns in the row FROM the
	// table WHERE we specify.  To increase the speed we could just SELECT the
	// strField instead of all of the columns (instead of *).
	char szSQLStatement[999] = {0};
	sprintf(szSQLStatement, "SELECT * FROM %s WHERE %s", strTable.c_str(), strWhere.c_str());

	// Open the database and run the SQL statement
	m_recordset->Open(szSQLStatement, m_connection.GetInterfacePtr(),
									  ADODB::adOpenForwardOnly, ADODB::adLockReadOnly, ADODB::adCmdText);
	
	// Now we create a _variant_t variable that stores different types of data.
	_variant_t var;

	// Using our recordset pointer, we access the GetValue() function by
	// passing in the column name into GetItem().  GetValue returns an
	// ambiguous _variant_t that we can cast to the appropriate type.
	var = m_recordset->Fields->GetItem(strField.c_str())->GetValue();

	// Set our value initially to -1
	int value = -1;

	// If we couldn't find the integer we don't want the program to crash so we
	// check if it is null, or "VT_NULL".  Remember, null means "nothing".
	if(var.vt != VT_NULL)
	{	
		// To get an integer value from our _variant_t object we use the intVal variable.
		// We need to cast or "convert" the value to a normal integer.
		value = static_cast<int>(var.intVal);
	}
	
	// Close the record set
	CloseRecordSet();

	// Return our extracted int
	return value;
}


// If we know the row number in the database we can use this function to get a string.
string CMyDatabase::GetString(string strTable, int index, string strField)
{
	// Select all the rows from the table
	char szSQLStatement[999] = {0};
	sprintf(szSQLStatement, "SELECT * FROM %s", strTable.c_str());

	// Execute the SQL statement
	m_recordset->Open(szSQLStatement, m_connection.GetInterfacePtr(),
								ADODB::adOpenForwardOnly, ADODB::adLockReadOnly, ADODB::adCmdText);
	
	// Now we are going to create a counter that will go through each row and
	// count up until we get to the desired row index.
	string strValue = "";
	int counter = 1;

	// We do a while loop until we reach the End of File (EOF) for our table.
	while(!m_recordset->ADOEOF)
	{
		// Create a var to store the current value of the current row's column.
		_variant_t var;
		var = m_recordset->Fields->GetItem(strField.c_str())->GetValue();

		// If we reached our desired index we can extract the string value and break.
		if(counter == index)
		{
			// If we couldn't find the string we don't want the program to crash so we
			// check if it is null, or "VT_NULL".  Remember, null means "nothing".
			if(var.vt != VT_NULL)
			{	
				// Cast the value as a string and break;
				strValue = static_cast<char *>(_bstr_t(var.bstrVal));
			}

			break;
		}

		// Increase the counter for each row we look at
		counter++;

		// This function moves us to the next row in the database
		m_recordset->MoveNext();
	}

	// Close the recordset and return our string value
	CloseRecordSet();
	return strValue;
}


// If we know the row number in the database we can use this function to get an int.
int CMyDatabase::GetInt(string strTable, int index, string strField)
{
	// Select all the rows from the table
	char szSQLStatement[999] = {0};
	sprintf(szSQLStatement, "SELECT * FROM %s", strTable.c_str());

	// Execute the SQL statement
	m_recordset->Open(szSQLStatement, m_connection.GetInterfacePtr(),
								ADODB::adOpenForwardOnly, ADODB::adLockReadOnly, ADODB::adCmdText);
	
	// Now we are going to create a counter that will go through each row and
	// count up until we get to the desired row index.  Here we set our initial
	// value to -1 meaning we didn't get a value yet.
	int value = -1;
	int counter = 1;

	// We do a while loop until we reach the End of File (EOF) for our table.
	while(!m_recordset->ADOEOF)
	{
		// Create a var to store the current value of the current row's column.
		_variant_t var;
		var = m_recordset->Fields->GetItem(strField.c_str())->GetValue();

		// If we reached our desired index we can extract the integer value and break.
		if(counter == index)
		{
			// If we couldn't find the integer we don't want the program to crash so we
			// check if it is null, or "VT_NULL".  Remember, null means "nothing".
			if(var.vt != VT_NULL)
			{
				// Cast the value as an integer
				value = static_cast<int>(var.iVal);
			}
			else
				value = -1;

			break;
		}

		// Increase the counter for each row we look at
		counter++;

		// This function moves us to the next row in the database
		m_recordset->MoveNext();
	}

	// Close the recordset and return our integer value
	CloseRecordSet();
	return value;
}


// This closes the current recordset
void CMyDatabase::CloseRecordSet()
{
	// Make sure we have a recordset pointer before checking if it's open.
	if (m_recordset)
	{
		// Check if the state is open, and if so, close it.
        if (m_recordset->State == ADODB::adStateOpen)
            m_recordset->Close();
	}
}


// This destroys our connection and current database pointers
void CMyDatabase::Shutdown()
{
	// Close the recordset if it's open
	CloseRecordSet();

	// If we have a valid connection pointer, close it and unset COM.
	if(m_connection)
	{
		m_connection->Close();
		CoUninitialize();
	}

	// Set our pointer variables to NULL
	m_connection = NULL;
	m_recordset = NULL;
}


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// This might be a lot of code to look at right now, but once you
// go through it all you can see that it is pretty straight forward.
// Working with database code can be sometimes frustrating because
// you can't easily debug what is happening once you send out SQL
// commands.  Sometimes they error and you don't understand why,
// even though you get a error with a description.
//
// We won't pretend to teach all about SQL in one tutorial, but
// you should check out the following website for some good examples
// of using SQL and the syntax:
//
// http://www.w3schools.com/sql/
//
// In order to really understand what this code is doing it is
// important that you open a database program and look at how the
// tables, rows and columns are created.  Otherwise it's hard to
// conceptualize what is going on in the code.
//
//
// © 2000-2006 GameTutorials 