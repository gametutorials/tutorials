#include "string_obj.h"

// Implementation of the default constructor
CStrObj::CStrObj()
{
	// Remember "mStr" is private, but we can directly access it because we are in the
	// scope of CStrObj.  You can only access private methods or member variables when you are 
	// inside the scope of the class that they belong to.
	
	// Set the first character in the array to NULL.  Since strings are NULL terminated in C++
	// this represents an empty string.
	mStr[0] = NULL;
}

// Implementation of the overloaded constructor
CStrObj::CStrObj(const char *str)
{
	// We already have a great method for setting a CStrObj, so we just
	// go ahead and call it using the passed in parameter.  Since we are
	// inside the scope of CStrObj, the program knows to call the "set" method
	// that is also inside the scope of CStrObj.
	set(str);
}

// Set the string object 
void CStrObj::set(const char *str)
{
	// Copy over the string
	strcpy(mStr, str);
}

// Print the string object
void CStrObj::print()
{
	cout << mStr;
};