#include "string_obj.h"

// The default constructor
CStrObj::CStrObj()
{
	// Initialize the pointer to NULL
	mStr = NULL;
}

// Copy constuctor
CStrObj::CStrObj(const CStrObj &strObj)
{
	/*
		What's going on here?  Nothing to special.  In general, the functionality of 
		the copy constructor and assignment operator are the same.  So instead of 
		typing the same code for both methods, we just call the assignment operator
		from the copy constructor.
	*/      
	*this = strObj;
		
	// **NOTE**	This implementation is shown only as an alternative that you may see
	//			in different code examples.  An equivalent implementation is:
	//
	//			CStrObj::CStrObj(const CStrObj &strObj)
	//			{
	//				set(strObj.mStr);
	//			}
}

// Assignment operator
CStrObj& CStrObj::operator =(const CStrObj &strObj)
{
	set(strObj.mStr);
		return *this;
}

// Sets the string object 
void CStrObj::set(const char *str)
{
	// First we need to get the length (minus the NULL character) of the
	// string we want to set
	unsigned int len = strlen(str); // Get length of "str"

	// If "mStr" is NULL, we need to allocate an appropriate length string
	if(!mStr)
	{
		// Allocate the string
		// The "+1" is for the NULL character at the end of the string
		mStr = new char[len + 1];
	}
	else // Otherwise "mStr" is already allocated.  We need to check if it's big
	{	 // enough to hold "str"
	
		// If "mStr" is smaller than or the same size as "str"
		if(strlen(mStr) <= len)
		{
			delete[] mStr; // Free up "mStr"
			
			mStr = new char[len + 1]; // Allocate a large enough string
									  // Remember the "+1" is for the NULL terminating character
		}
	}
	
	// Copy over the string
	strcpy(mStr, str);
}

// Print the string object
void CStrObj::print(int numBlnakLines)
{
	cout << mStr; // Print the string
	
	// Print "numBlankLines" after printing the string 
	for(int i = 0; i < numBlnakLines; ++i)
		cout << endl;
};

// The deconstructor.  It is responsible for freeing up any dynamic
// memory allocated by the class
CStrObj::~CStrObj()
{
	// If "mStr" is not NULL, it must point to dynamic memory we
	// allocated and thus needs to be deleted 
	if(mStr)
	{
		delete[] mStr; // Delete the memory
		mStr = NULL; // This is a good software development habit to get into.  
					// ALWAYS set pointers to NULL, after you free the memory they point to.
	}
	
	// We print out this message to help visualize when destructors get called
	// in a program.
	cout << "Destructor called" << endl;
}