#include "string_obj.h"

/*
	Now it is time to implement the C++ class.  Lets first examine the syntax of our
	first method in greater detail.
	
	void CStrObj::set(const char *str)
	 |		|	   |		  |
	 |		|	   |		  |
	 |		|	   |		  ---- "const char *str" The input argument(s) to the method
	 |		|	   |	   			
	 |		|	   ---- "set" The user defined name of the method
	 |		|	   
	 |      ---- "CStrObj::" The function "set" belongs to the scope of the class "CStrObj".
	 |						 The :: (scope resolution operator) associates the method "set" with
	 |						 the class "CStrObj".  This way different classes could each have
	 |						 the method "set". 
	 |					      
	 ----- The return argument of the method
*/

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

// ******************************* //
// *** C Style Implementation *** //
// ***************************** //

// Set the string
void Set(SString *s, const char *theString)
{
	if(s) // If the pointer doesn't equal NULL (i.e. is assumed to be valid)
	{
		// Copy over the string
		strcpy(s->str, theString);
	}
}

// Print the string
void Print(SString s)
{
	printf(s.str);
}