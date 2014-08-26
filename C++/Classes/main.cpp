// Done by TheTutor

/*  
	This is the first part of a five part series to define what is meant by a "class" in C++.  
	First, a bit of review... The C++ language is an OOP (Object Oriented Programming) language.
	For a language to be labeled OOP, it must meet three critera:  encapsulation, inheritance, 
	and polymorphism.  Part one of this series is going to define encapsulation and how it
	is accomplished through classes in the C++ language.
	
	Encapsulation is the technique of binding together data structures and the functions (methods)
	that operate on them.  A class is a prototype for an object (one of the reasons it's called
	object oriented programming...) which contains variables and methods that operate on the variables.
	The variables dictate the state of the object and the methods dictate the behavior of the object.
	
	Lets step back and look at a strictly C approach.  In C Land, you declare a struct to hold data and
	then functions which act on the data and do different things.  In C++, through encapsulation, 
	we combine the functions and data into an object.  Then we can declare an instance of that object
	and invoke its methods to interact with the object's data.
	
	Isn't tech-babble fun!  To help illustrate the concept of what a class is, this tutorial is going
	to define a simple class and then use it main().  Additionally, the C style code (procedural programming
	approach) to do the exact same thing will be provided so you can see two different, but equally valid,
	approaches to accomplishing the same task.
	
	Off to class...
*/

#include "string_obj.h"

// The main()
int main()
{
	// **************************** //
	// *** C++ Version ************ //
									//
	CStrObj theString;				//
									//
	theString.set("Hello World");	//
	theString.print();				//
									//
	// **************************** //
	// **************************** //
	
	// Blank Lines
	cout << endl << endl;
	
	// ************************ //
	// *** The C version ****** //
								//
	SString s;					//
								//
	Set(&s, "Hello World");		//
	Print(s);					//
								//
	// ************************ //
	// ************************ //
	
	// Blank Lines			
	cout << endl << endl;
		return 0; // Return zero for "zero errors with the program"
}


/*	Class is not yet over...

	Lets do a recap of the important points from this tutorial:
	
		1)  Encapsulation is the binding together of methods and variables.  In C++ this binding
			occurs under the scope of a class.
			
		2)  Every method and member variable in a class is either public, protected, or private.
			If it it public, that means it is publicly assessable by an instance of the class.
			
		3)  The :: (scope resolution operator) is used when implementing a method in a class to
			make sure the method is part of the class.  This way we can have several methods with
			the exact same name and parameters, yet potentially completely different functionality,
			be methods in different classes.
			
	If you are new to the concept of a class, be sure to read all five tutorials on the subject.  If 
	you have any questions, feel free to post 'em at www.GameTutorials.com.
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2005 GameTutorials |
\*-------------------------*/
