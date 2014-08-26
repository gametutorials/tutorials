// Done by TheTutor

/*  
	This is the second part of a five part series to cover what is meant by a "class" in C++.  
	This tutorial is going to cover what a "constructor" in a class is and take a look at the 
	access specifier "private".  
	
	A constructor is a method that is automatically called when an object is created.  
	Lets take a look at a coding example:
	
		int main()
		{
			CStrObj s; // Here we declare a variable 's' which is of type "CStrObj".  Upon
					   // declaration, the constructor is called for the variable 's'.
					   
			return 0;					   
		}
		
	The full uncensored details of constructors and the keyword "private" can be found in 
	"string_obj.cpp" and "string_obj.h".
		
	Now, lets head off to class...
*/

#include "string_obj.h"

// The main()
int main()
{
	// Create an instance of a string object
	// The default constructor is called
	CStrObj s1;				
									
	s1.set("Hello World"); // Set the string object	
	s1.print(); // Print the string object to the screen				 
									
	// Blank Lines
	cout << endl << endl;
	
	// Create a different instance of a string object
	// using the overloaded constructor
	CStrObj s2("String #2");
	
	s2.print(); // Print the string object to the screen				
										
	// Blank Lines			
	cout << endl << endl;
		return 0; // Return zero for "zero errors with the program"
}


/*	Class is not yet over...

	Lets do a recap of the important points from this tutorial:
	
		1)  Classes have four default methods, the constructor, copy constructor, assignment
			operator, and destructor.  You can chose to overload each default method or use
			the default.
			
		2)  The default constructor takes no arguments.  It's commonly used to provided
			default values for all the member variables of a class. 
			
		3)  The keyword "private" makes all methods and member variables labeled private accessibly
			only inside the scope of the class.
			
	This concludes part 2 of the 5 part series on classes.  If you skipped part 1, shame on you %)
	
	If you have any C/C++ questions be sure to post 'em in the appropriate forum found at:
	www.GameTutorials.com
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2005 GameTutorials |
\*-------------------------*/
