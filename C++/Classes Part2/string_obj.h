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
	// Our first access specifier is public, because we want to be able to call the
	// following methods
	public:
		
		/* 
			Default Constructor
			
			When you create a class prototype you get four methods by default.  They are
			a constructor, copy constructor, assignment operator, and destructor.  This tutorial
			only talks about the first one, the constructor, but just realize that by default you get
			all of these methods.  Now of course, you can write your own versions of these methods,
			overriding the default, or you can just use the default.  Depending on what the class
			is meant to do will determine if you write your own or use the default methods.
			
			The default constructor does not take any arguments.  It is used to give the member
			variables of your class a default value.  If you choose not to overwrite it, your member
			variables will be assigned random values.
			
			There is no return value from a constructor.  So if an object fails when you are 
			constructing it, you can not return a flag to say there has been an error.     
		*/
		CStrObj(); // We are overwriting the default constructor and providing our own implementation
		
		/*
			Overloaded constructor
			
			As mentioned above, the default constructor takes no parameters.  However, you can 
			overload the constructor, and provided as many variations as you like, provided they
			follow the rules of C++ function overloading.
			
			Here we are creating a constructor that takes a const char* as an argument and sets
			the CStrObj to the argument passed in.
		*/
		CStrObj(const char *str);    
	
		void set(const char *str); // Sets CStrObj to "str"
		void print(); // Prints CStrObj
	
	/*
		Below we use the access specifier "private".  Remember that an access specifier stipulates 
		the allowable access to a group of methods and variables inside of class.  
		
		When you think of the keyword "private" you think of "remaining private" or "unaccessible to 
		anyone besides yourself".  Any function or variable that is private can not be accessed by an 
		instance of the class.  Lets give a quick example:
		
			class CClass
			{
				public:
					int mInt;
				
				private:
					float mFloat;
			};
			
			int main()
			{
				CClass c;
				
				c.mInt = 50; // Since "mInt" is public, this is allowed
				
				c.mFloat = 25.0f; // Since "mFloat" is private this is not allowed
								  // This would give a compiler error
				
				return 0;
					
				**NOTE** The same holds true for methods.					
			}
			
		The private keyword really makes encapsulation, one of the three necessary components of an
		object-oriented programming language, possible.  In general, the reason to create a class, is
		so that you can provide a set of well defined methods.  You do not want	a user of the class 
		to directly manipulate the member variables, so you make them private.  This way, they can only
		interact with the member variables by calling your predefined functions.  
			
		So to recap... When you create a class prototype, any method or member variable that is 
		private can only be accessed within the scope of the class.		 
	*/
		
	private: // All methods and member variables following will be "private"
	
		char mStr[kMaxStrLen]; // The character array holding the literal string
};

#endif