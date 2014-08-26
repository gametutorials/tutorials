// Done by TheTutor

/*  
	This is the fifth part of a five part series to cover what is meant by a "class" in C++.  
	This tutorial is going to cover what a "destructor" in a class is and how to deal with
	dynamic memory in a class.
	
	A destructor is a method that is automatically called when an object is deleted either manually
	(by calling delete) or automatically when an object looses scope (local variables).
	
	Lets take a look at a coding example:
	
		int main()
		{
			CStrObj *s = NULL; // A pointer to a CStrObj
			
			s = new CStrObj; // Dynamically create a CStrObj on the heap
			delete s; // Free up the memory.  This where the destructor gets called!
			
			return 0;					   
		}
		
	The full scoop on destructors (also called deconstructors) can be found in 
	"string_obj.cpp" and "string_obj.h".
		
	Time to finish this series :)
*/

#include "string_obj.h"

// The main()
int main()
{
	CStrObj s1, s2, s3;
	
	// Set the strings
	s1.set("Small string");
	s2.set("A medium sized string");
	s3.set("A larger string than the small string");
		
	// Here we test scope and show how a variable, when it goes out of scope, it's
	// destructor is called
	{
		CStrObj test;
		test.set("Testing...");
		test.print(1);
		
	} // As soon as we leave this scope the destructor for "test" is called
	
	cout << endl << endl; // Print some blank lines
	
	s2 = s1; // Test copying from smaller string to larger string
	s1 = s3; // Test copying from larger string to smaller string
		return 0; // Return zero for "zero errors with the program"
}

/*	Finally class is over!

	Lets do a quick recap of this tutorial:
	
		1)  The destructor gets called when an object is deleted or goes out of scope
		
		2)  You should always implement the destructor if you class has dynamic data
		
	And that wraps up this five part series.  If you have any C/C++ questions, don't hesitate
	to post at www.GameTutorials.com
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2005 GameTutorials |
\*-------------------------*/
