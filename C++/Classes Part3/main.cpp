// Done by TheTutor

/*  
	This is the third part of a five part series to cover what is meant by a "class" in C++.  
	This tutorial is going to cover what a "copy constructor" in a class is all about.  When 
	reading C++ syntax tutorials, things can get a bit dry.  There's no way to get around
	it, sometimes you just have to swallow the hard technical facts.  That is why you should think
	about a nice juicy steak, cooked to perfection, when reading tech-heavy tutorials.  That should
	wet your appetite and keep everything nice and tasty.
	
	A copy constructor acts a lot like a constructor.  It is responsible for constructing
	(initializing) an object.  A constructor is called when an object is declared, but a copy
	constructor is called in three separate situations.
	
		1)  When a class is passed as a parameter to a function.
		2)  When a class is returned from a function.
		3)  When a class is initialized to another value when it's being declared.
		
	Lets look at a coding example for each situation.
	
		1)  When a class is passed as a variable to a function. 
	
			void SomeFunction(CStrObj s)
			{
				// Function does something...
			}
		
			int main()
			{
				CStrObj strObj;
				
				// When a class is passed as a parameter to a function it's copy constructor is 
				// called.  This is because for any parameter passed to a function, a temporary variable
				// that is a copy of the variable passed in, is created.  The function then uses the copy 
				// and when the function is over, the copy is destroyed.  It works this way because this
				// is the nuts and bolts of how C++ works.  
				SomeFunction(strObj);
					return 0;			
			}
			
		2) When a class is returned from a function.
		
			// Here we have a function that returns the variable type "CStrObj"
			CStrObj MakeHelloString()
			{
				CStrObj s;
				
				s.set("Hello!");
				
				// It all comes down to this return statement.  Since we want to return
				// this class to whatever called this function, it has to be copied onto the stack.
				// This copying takes place by the copy constructor.
				return s;
			}
			
			int main()
			{
				CStrObj result = MakeHelloString();
					return 0;
			}
			
		3)  When a class is initialized to another value when it's being declared.
		
			int main()
			{
				CStrObj str1("Hi");
				
				// You would think at first glance this should call the assignment operator (by the way
				// the '=' is the assignment operator), but alas it does not.  The code below calls the
				// copy constructor, because "str2" has not been constructed yet.  When you declare an
				// object a constructor will be called!
				CStrObj str2 = str1;
					return 0;
			}
				 
	Check out our favorite "string_obj.h" and "string_obj.cpp" for the rest of the details.
		
	The bell has rung, it's time for class...
*/

#include "string_obj.h"

// Prints a CStrObj on a line, then prints a blank line
void PrintString(CStrObj strObj);

// The main()
int main()
{
	// Create an instance of a string object
	CStrObj s1;				
									
	s1.set("Steak"); // Set the string object	
	
	// Create another instance of a string object
	// The copy constructor will be called
	CStrObj s2 = s1;
	
	// Set the "s1" to a different value 
	s1.set("Who is hungry for some...");
	
	// Print both strings
	PrintString(s1); // The copy constructor for "s1" will be called
	PrintString(s2); // The copy constructor for "s2" will be called
		
	return 0; // Return zero for "zero errors with the program"
}

// Once again %)
// Prints a CStrObj on a line, then prints a blank line
void PrintString(CStrObj strObj)
{
	cout << strObj.get();
	cout << endl << endl;
}

/*	Class is not yet over...

	Lets do a recap of the important points from this tutorial:
	
		1)  Copy constructors get called in three areas of code:
		
				A)  When a class is passed by value to a function/method
				B)  When a class is returned from a function/method
				C)  When a class is used to initialize another class being declared.
			
		2)  You should ALWAYS override the copy constructor if your class has dynamic data.
			Otherwise it's probably okay to leave the default. 
			
	This concludes part 3 of the 5 part series on classes.
	
	If you have any C/C++ questions be sure to post 'em in the appropriate forum found at:
	www.GameTutorials.com
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2005 GameTutorials |
\*-------------------------*/