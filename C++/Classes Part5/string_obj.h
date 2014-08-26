#ifndef STRING_OBJ_H
#define STRING_OBJ_H

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
using namespace std;

// Create the prototype for the class
class CStrObj
{
	// Our first access specifier is public, because we want to be able to call the
	// following methods
	public:
	
		// Constructor
		CStrObj();
		
		// Copy Constructor
		CStrObj(const CStrObj &strObj);
		
		// Assignment Operator
		CStrObj& operator =(const CStrObj &strObj);
		
		void set(const char *str); // Sets CStrObj to "str"
		void print(int numBlankLines); // Prints CStrObj then prints "numBlankLines" blank lines
		
		/*
			Destructor
			
			Here it is!  The destructor is an method that every class gets by default.  It 
			is called when ever the object is destroyed.  In general there are two ways an
			object gets destroyed.  One is way is when you explicitly call "delete" on the object.
			The other is when the object looses scope.  A scope is formed by a left bracket '{' and
			a right bracket '}'.  So given this example:
			
			void SomeFunction()
			{ // This starts the scope of the function
				
				CStrObj s;
				
				s.set("GameTutorials teaches programming through the art of making games");
				s.print(1);
			} // This ends the scope.  At this point the local variable 's' would be
			  // destroyed, thus it's destructor would be called
			  
			If your class does not have dynamic data, then you can most likely can just use the
			default destructor.  If you have dynamic data then you MUST implement your own 
			destructor.  Why you ask?  To properly free up the memory that's why.  The destructor's
			job in life is to destroy stuff.  If you don't overload it, it will have no job
			and then it will apply for welfare and nobody wants that!
			
			The destructor does not take any parameters ever.  It also never returns any
			parameters.  It looks a lot like the signature for the default constructor
			except it begins with the tilde character '~' 			
		*/
		~CStrObj();    
		
	private:
	
		// A char* to hold our string
		char *mStr;		
};

#endif