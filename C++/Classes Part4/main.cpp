// Done by TheTutor

/*  
	This is the fourth part of a five part series to cover what is meant by a "class" in C++.  
	This tutorial is going to talk about the assignment operator (the = operator) and what 
	the "this" pointer is. 
	
	The default assignment operator works in the way you would expect.  It sets one class equal
	to another class.  Lets look at when it gets called in code:
	
		int main()
		{
			CStrObj str1("Hi"); 
			CStrObj str2("How are you");
			
			str1 = str2; // Assignment operator is called
				return 0;
		}
		
	For all the gory details, head now to "string_obj.h" and "string_obj.cpp".
*/

#include "string_obj.h"

// The main()
int main()
{
	// Create two strings with the overloaded constructor
	CStrObj s1("You can get with this...");
	CStrObj s2("Or you can get with that...");
	
	// Assignment operator method is called.  "s1" is assigned to "s2".  
	// Both strings are now equal
	s2 = s1;
	
	// Don't believe it?  See for yourself
	s1.print();
	s2.print();
		
	return 0; // Return zero for "zero errors with the program"
}

/*	Class is not yet over...

	Lets do a recap of the important points from this tutorial:
	
		1)  The assignment operator (the = operator) is called when you set
			two classes equal to one another.
			
		2)  You should ALWAYS override the assignment operator if your class has dynamic data.
			Otherwise it's probably okay to leave the default.
			
		3)  The "this" pointer points to THIS (the current) instance of a class.  Using the
			"this" pointer you can access data and methods contained in a class.  A good reason
			to use the "this" pointer is for the return value from the assignment operator. 
			
	This concludes part 4 of the 5 part series on classes.
	
	If you have any C/C++ questions be sure to post 'em in the appropriate forum found at:
	www.GameTutorials.com
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2005 GameTutorials |
\*-------------------------*/