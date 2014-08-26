#include "errorLog.h"

// First and foremost, we initialize our static member variable.
// Why do we have the instance of CErrLog static?  Well, static member variables are shared 
// amongst all instances of a class.  So this insures there will be one and only copy 
// of these variables around and all classes will have the same value for these variables.
CErrLog* CErrLog::mInstance = NULL; 
								   
// Here we create an instance of the CErrLog
void CErrLog::create()
{
	if(mInstance == NULL)
		mInstance = new CErrLog;
}

// Open the error log file specified by "file_name"
void CErrLog::openLog(const char *fileName)
{
	// Error Check
	if(!fileName)
		return;

	closeLog(); // Don't want to try and open a error log file if one is already open,
			    // so close any open log first.

	// Here's where we open the file so we must be careful to open it in "append" mode so
	// we don't overwrite anything.
	mFileOut.open(fileName, ios_base::out | ios_base::app);

	// Error Check
	if(!mFileOut)
		cout << "Couldn't open error log" << endl; 
}

// Closes the error log file
void CErrLog::closeLog()
{
	// If the file is open
	if(mFileOut.is_open())
		mFileOut.close(); // Close it
}

void CErrLog::logError(const char *err)
{
	// Error Check
	if(!err)
		return;

	mFileOut << err << endl; // Write out the error to the file
}

// Free up all of the memory associated with the singleton
void CErrLog::destroy()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = NULL;
	}
}
