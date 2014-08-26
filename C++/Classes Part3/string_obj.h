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

// Create the prototype for the class
class CStrObj
{
	// Public specifier because we want to be able to call the following methods
	public:
	
		// Constructors
		CStrObj();
		CStrObj(const char *str);
		
		/*
			Copy Constructor
			
			The signature for a copy constructor always looks like this:
			
				CStrObj(const CStrObj &strObj)
				  |				|
				  |				|
				  |				--- One parameter passed which is a constant reference to the class 
				  |
				  --- Name of class
				  
				  **NOTE** Copy constructor's do not return anything
				  
			The default copy constructor copies every member variable contained within
			the class.  So lets look at a small program and see how that plays out:
			
				class CPoint
				{
					public:
						float x, y;
				};
				
				int main()
				{
					CPoint pt1;
					
					pt1.x = 15.0f;
					pt1.y = 25.0f;
					
					CPoint pt2 = pt1; // Copy constructor gets called
									  // In pseduo code it would like like this:
									  //	{
									  //		pt2.x = pt1.x;
									  //		pt2.y = pt1.y;
									  //	}
					return 0;
				}
				
			As you can see from the above example the default copy constructor, in general, does
			exactly what you would want it to do.  However, if your class has any dynamic data you 
			need to write your own copy constructor.  The CStrObj class, since it's data is
			not dynamic but rather a fixed size char array, does not need to have the copy constructor
			overwritten but we are going to do it anyway.  You can always write your version of
			the copy constructor.   
		*/
		CStrObj(const CStrObj &strObj); // The copy constructor
	
		/*
			As mentioned in part 1 of this series, encapsulation combines data and methods
			that act on that data into one object.  When using encapsulation (so anytime
			you declare a class) another software development practice is typically used.
			Referred to as "setters" and "getters", a class typically provides methods
			for setting it's data and methods for getting it's data.  Data returned
			by the "get" method, if modified, does not change the value stored in the
			class.  The only way to change the data in the class is by the "set" method.
			
			So why do this?  Well there are a couple reasons:
			
				1)  Through the "set" method the class controls how data is changed.
					The "set" method can then make sure only valid data is set.
				2)  The "get" method can make sure that data is passed back to a user
					of the class in a suitable format.  Any data the user doesn't need or
					should be hidden doesn't need a "get" method.
				3)  Because the data of the class can't be directly accessed, it's possible
					to completely change the underlying data types.
					
			Lets look at a mini coding example to illustrate some of the above reasons:
			
				// Class to hold a 2D vector that does NOT
				// use "setters" and "getters"
				class CVec2
				{
					public:
						float x, y;
				};
				
				int main()
				{
					CVec2 vector;
					
					vector.x = 4.0f;
					vector.y = 8.0f;
						return 0;
				}
				
				// Now if we change the class to this
				class CVec2
				{
					public:
						float vec[2];
				};
				
				// Now the main wouldn't compile.  Imagine CVec2 is used hundreds of times
				// throughout a program and you have a lot of compiler errors to fix!
		*/
	
		// Our "setter" and "getter" method
		void set(const char *str); // Sets CStrObj to "str"
		const char* get(); // Gets the value of the string
	
	// Private specifier because we do not want direct access to the following
	// member variable	
	private:
	
		char mStr[kMaxStrLen]; // The character array holding the literal string
};

#endif