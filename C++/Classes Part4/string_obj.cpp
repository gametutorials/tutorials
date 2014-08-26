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

// Copy constructor
CStrObj::CStrObj(const CStrObj &strObj)
{
	set(strObj.mStr);				 
}

/*
	First up, why pass "const CStrObj &strObj" into the method?  Well there a couple reasons, first
	when we pass a class by reference or by pointer, it does not, make a copy of the object.  For
	a class with any significant size, this is a huge bonus.  The "const" portion then says, "This
	object is constant and can not be altered by this method."  Besides obviously making sure
	the passed in parameter doesn't get changed, it allows the compiler to potentially optimize
	things because it knows it can't be changed.
	
	Second, why return "CStrObj&" ?  Well when we overload the assignment operator we
	want it to work like the normal assignment operator.  The assignment operator always
	works right to left.  So in this example code:
	
		int x, y, z;
		x = y = z = 0;
		
		// 0 is assigned to z
		// z is assigned to y
		// y is assigned to x
		
	To make our class works the same way, we have to return a copy of the class that was just set.
	We return by reference for the same reason we pass by reference.  If the class is large 
	in it's byte size, returning by reference will be a big speed increase.  We could potentially
	alter the values of the class since we are returning by reference, but, since assignment
	only happens from right to left that's not possible.
*/
CStrObj& CStrObj::operator =(const CStrObj &strObj)
{
	// Use the trusted "set" method to set the string
	set(strObj.mStr);
	
	/*
		What is this (no really...)?   The "this" pointer is the pointer that points
		to "this" instance of a class.  Lets see if a simple code example helps:
		
			class CVec2
			{
				float mX, mY;
				
				void set(float x, float y)
				{
					this->mX = x;	// Set mX
					mY = y;			// Set mY
					
					// Check out the sweet new syntax.  In this example
					// "this->mX" and, what you are used to, "mY" are two 
					// different ways to refer to class variables.  Ultimately,
					// the functionality of the syntax is equivalent.  Thus,
					// typing "this->mX" is the same as typing in "mX".							
				}
			};
			
			int main()
			{
				CVec2 vector1;
				CVec2 vector2;
				
				// When "set" gets called the code would jump to the implementation
				// a few lines above.  
				
				vector1.set(2.0f, 4.0f);
				// For the above function, "this" equals "vector1"
										
				vector2.set(16.0f, 32.0f);
				// For the above function, "this" equals "vector2"
				
				return 0;
			}
			
		So to recap... "this" points to whatever instance of the class a method just
		got called for.
	*/  
	
	return *this;
	/*
		Okay, so we're inside of the assignment operator, and "this" is a pointer to
		the the instance of the class that called the assignment operator.  If our method
		returned a "CStrObj*", we could just return "this".  But it doesn't, it returns a 
		reference "CStrObj&".  Thus we return "*this" to make it return the correct type.
	*/
}

// Set the string object 
void CStrObj::set(const char *str)
{
	// Copy over the string
	strcpy(mStr, str);
}

// Get the string
// **NOTE** For extra details on this method, see the bottom of this file
const char* CStrObj::get()
{
	return mStr;
}

// Print the string
void CStrObj::print()
{
	cout << mStr << endl;
}

/*
	**NOTE** This explains the "get" method and it's implementation

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
		}	
*/