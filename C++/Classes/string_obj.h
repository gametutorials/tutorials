#ifndef STRING_OBJ_H
#define STRING_OBJ_H

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
using namespace std;

// The maximum length a string can be
const int kMaxStrLen = 256;

/*
	Lets cover the syntax for declaring a class.  First you'll notice the keyword "class".
	Similar to "struct", you use the keyword "class" to begin creating a class prototype.
	Next is the user defined name of the class.  Here we use "CStrObj" as our class name.
	And lastly, we begin the class prototype with a '{' (left curly bracket).  
	
	Once the prototype and implementation of CStrObj is finished, we can declare
	variables of type CStrObj and use them in our program.
*/
class CStrObj
{
	/*
		Next up in the declaration of a class is this keyword "public".  This commonly called an
		"access specifier".  An access specifier stipulates	the allowable access to a group of methods
		and data inside of class.  There are three access specifiers for a class: public, protected, and
		private.  Later tutorials will talk about "protected" and "private".  For now lets focus on the
		"public" keyword.
		
		When you think of the keyword "public" you think of being in the public domain.  Any
		function or variable that is public can be accessed by an instance of the class.  Lets 
		give a quick example:
		
			class CClass
			{
				public:
					int mInt;
					float mFloat;
			};
			
			int main()
			{
				CClass c;
				
				c.mInt = 50; // Since "mInt" is public, this is allowed
				c.mFloat = 25.0f; // Since "mFloat" is public it can be directly accessed
					return 0;
					
				**NOTE** The same holds true for methods.					
			}
			
		So to recap... When you create a class prototype, only the methods and member variables that
		are public are accessible when you declare a variable of the class type.		 
	*/

	public: // This is an access specifier.  Every function and variable below, until the end
			// of the class or a new access specifier, will be "public".   
	
		void set(const char *str); // Sets CStrObj to "str"
		void print(); // Prints CStrObj
		
		// **NOTE** Functions declared inside of a class are typically called "methods".
	
		char mStr[kMaxStrLen]; // The character array holding the literal string
		
		// **NOTE** Variables declared inside of a class are typically called "member variables".

}; // A class prototype is finished with a '}' (right curly bracket) and a semi-colon

// ***************************** //
// *** C Style Declarations *** //
// *************************** //

// Create a string structure
struct SString
{
	char str[kMaxStrLen];
};

void Set(SString *s, const char *theString); // Sets the SString
void Print(SString s); // Prints the SString

#endif