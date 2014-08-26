// Done by TheTutor

// Say for instance you wanted to create a 3D point class that contained an (x,y,z).  Now, 
// sometimes you want the (x,y,z) of the point class to be of type integers.  Other times you
// want the (x,y,z) of the point class to be of floats, and even some other times you want
// the (x,y,z) to be doubles.  Wouldn't it be handy if you could just write ONE class that 
// handled all of your needs???
//
// Well, with C++ template classes you can do just that.  We are going to implement a 
// class named "CPoint" which will tackle the problem purposed above.
//
// Before we jump into the code, lets look at some template facts.

// 1)	Will start with the syntax.  Below is the CPoint class defined without 
//		anything it.
//
//		template <typename type>
//		class CPoint
//		{
//		};
//		
//		So lets break this down:
//	
//		template <typename type>
//		  |
//		  |
//		  --- This is the keyword "template".  This says, "Hey compiler.  Whatever follows is going
//			  to be a template."  Notice that after the keyword template, angled brackets (< and >) are 
//			  used to enclose the template definition.
//
//		template <typename type>
//					|
//				    |
//				    --- This is the keyword "typename".  It is used to declare a generic name (in 
//						this case we used "type") that will be used to declare variables inside of
//						the class.  Lets add some variables to the CPoint class so this can	become
//						more clear.
//						
//						template <typename type>
//						class CPoint
//						{
//							type x;
//							type y;
//							type z;
//						};
//						
//						When we declare a CPoint we get to specify what "type" will be.  The 
//						name "type" is completely arbitrary.  The Standard Template Library (STL)
//						uses the name "T" instead of "type".  So using that convention, CPoint would
//						look like this:
//
//						template <typename T>
//						class CPoint
//						{
//							T x;
//							T y;
//							T z;
//						};

// 2)	Templates MUST be implemented in the header file.
//		**NOTE** Using some #include tricks and .inl files, you can give the appearance
//				 of the implementation code of a template class as being located outside of 
//				 the header file, but at the end of the day it must be in the header file.
//
// 3)	If a class template has a function that is NEVER used, it will NOT be 
//		compiled.  Thus it is good practice when creating a template class to also
//		create a test app to ensure each function works properly.

#include "point.h"
#include <iostream>
using namespace std;

int main()
{
	// Notice the syntax when you declare a instance of type CPoint.
	// First comes the class name CPoint.
	// Second, in-between the angled brackets, comes the variable type you
	// wish to have the CPoint class take on.
	// Third comes the user defined variable name.
	// So you can think of it like this:  When you declare a templated class, whatever
	// you put between the <>'s will get substituted in your class everywhere "type" appears.
	// Thus for this declaration:  CPoint<int> p1;
    // the x, y, and z of p1 will all be of type int.

	CPoint<int> p1; // Use the default constructor
	CPoint<int> p2(5,4,3);
	CPoint<float> p3(2.0f, 3.0f, 4.0f); // Use the overloaded constructor
	CPoint<double> p4(1.5, 2.5, 3.5); // Use the overloaded constructor

	// Use the "set" function
	p1.set(10,11,12);

	CPoint<int> result = p1 + p2; // Use the overloaded + operator
								 // It should equal (15,15,15)

	// For fun print out the result
	cout << result.x << " " << result.y << " " << result.z << endl;
	return 0;

	// !MENTAL CHALLENGE!
	// Would the following code compile?
	/*
		CPoint<int> = p2 + p3;

		// Fact 5 below answers the question...
	*/
}

// Some quick facts summed up in a nice easy to read template:

/*
	1)  In general you the first line for declaring a templated class and all the functions in
		the templated class are like so:

		template<class YOUR_VARIABLE_NAME>

		YOUR_VARIABLE_NAME could equal pretty much anything.  
		Some examples are: item, T (this is pretty standard), genVar, cookieMonster, etc

	2)	Templated classes MUST be implemented in the same .h file in which they are defined.

	3)	If you DO NOT call a function in your program, that is in your templated 
		class, it WILL NOT be compiled.

	4)  When returning a variable from a function, that is of the templated class type
		you use this syntax:

		CLASS_NAME<templateVarName> // Return type

		When passing in a variable into a templated class function OR
		declaring a variable inside a templated class function that is of the
		templated class type you use "normal" syntax:

		CLASS_NAME yourVariable // Parameter to function OR variable declaration
							    // inside of templated class.

	5)  ... And the answer to mental challenge above is NO!  This is because
		The types are different.  If we wanted that to compile, we would
		have to overload the + operator again.  Try adding the code to
		make it compile.
*/

// Hopefully you learned a good deal about templates.  As always,
// post your questions and comments at www.GameTutorials.com.

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/
