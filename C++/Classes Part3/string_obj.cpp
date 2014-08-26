#include "string_obj.h"

// Default constructor
CStrObj::CStrObj()
{
	mStr[0] = NULL;
}

// Overloaded constructor
CStrObj::CStrObj(const char *str)
{
	set(str);
}

// The copy constructor
// First why do we pass a "const CStrObj &strObj" ???
// Lets look at each part of the input parameter and explain why it's there.
// The "const" is there, because we want the parameter to be constant.  Or in other words,
// we do not want the copy constructor to be able to change the input parameter.
// We passing an object of the same type (in this case CStrObj) because the whole point
// is to make a copy of an object.  We pass by reference (remember the '&' means passing 
// by reference) so that we can just pass the pointer to the object.  In a 32-bit machine
// this means we'd only pass 32-bits of data which is fast.  Plus we already said when we
// pass an object by value (ie not by pointer or reference) the copy constructor gets called.
// This would cause a never ending chain of function calls causing our computer to explode!
CStrObj::CStrObj(const CStrObj &strObj)
{
	// We are inside of CStrObj and we have passed a CStrObj in as as argument
	// Since we're living inside the scope of CStrObj, we can directly access 
	// the passed in variable "strObj" private data members.  By doing that
	// we'll call our ever faithful function "set" to set the CStrObj.
	set(strObj.mStr);
}

// Set the string object 
void CStrObj::set(const char *str)
{
	// Copy over the string
	strcpy(mStr, str);
}

// Gets the string
const char* CStrObj::get()
{
	return mStr;
}

/*
	Explanation of the "get" method...

	Since "mStr" is private, we can not directly access the character array.  Of course on the
	other hand, we might want to do stuff with it, like print it for example.  The "get" method
	gives us the best of both worlds.  We can use the return value (const char*) and because it's
	"const" we can't change it.  So for example this code would be legal:
	
		// Valid code
		int main()
		{
			CStrObj strObj("Hello");
			
			const char* s = strObj.get(); // Totally legal
			
			cout << s << endl; // No problems here
				return 0;
		}
		
	On the other hand, this would not be legal:
	
		// BAD CODE!!!  Will not compile
		int main()
		{
			CStrObj strObj("Hello");
			
			const char* s = strObj.get(); // This is totally valid
			
			s[0] = 'J'; // Can't do this since it's "const" and can't be changed
			
			char *s2 = strObj.get(); // Can't do this because the method returns "const char*" so
									 // it can only be set to a "const char*"
									 
			return 0;
		}	
*/