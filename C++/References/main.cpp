// Done by TheTutor

/*  
	**NOTE** This tutorial assumes you have a working knowledge of pointers.

	References are one of the great things you get with C++, but not with C.
	They act very similar to pointers, but in some people's opinions are easier to use.
	In C++ when you see the & ("reference to") in a function declaration it means that 
	function takes a reference to a variable.  Typically you want to pass an argument by pointer 
	to a function so the function could change it's value.  Well, when you pass something as a 
	reference it's the same concept, you are passing the address of that variable.  This means if 
	you pass something by reference into a function, that function can change it, and when the
	function returns, that variable will be modified just in the same way as if you would
	of passed it by pointer.

	So what is the difference?  Well to boil it down, references can not be invalid, pointers can.
	This point will showcased by some code in main!
*/

#include <iostream>
using namespace std;

// Below are two functions that swap the value of two integers.  One uses pointers,
// the other uses references
void SwapWithPointers(int *num1, int *num2); 
void SwapWithReferences(int &num1, int &num2);

// Prints the values of num1 and num2 to the screen on separate lines
void PrintNumbers(int num1, int num2);

int main()
{
	int num1 = 52;
	int num2 = 44;

	cout << "Initial values of num1 and num2" << endl;
	PrintNumbers(num1,num2);

	// Lets take a look at some cases where a pointer could be invalid.
	// A pointer can be NULL which simply points to nothing.  So uncommenting
	// the function below, will result in a big CRASH!!!!
		//SwapWithPointers(&num1, NULL); // Crash with NULL pointer

	// Also a pointer may be valid, but what it points to is not.  We recall that
	// pointers are an address in memory that point to something.  So we'll pass 
	// an address in memory,  but we have no way of knowing what it points to.
	// This will most likely CRASH!!!
		// SwapWithPointers(&num1, (int*)0x01234567); // Crash with valid pointer, invalid data

	// Swap the numbers with a valid pointers
	SwapWithPointers(&num1,&num2);
	cout << endl << "After calling SwapWithPointers() on num1 and num2..." << endl;
	PrintNumbers(num1,num2);

	// Okay we said references are always valid.  But is that really true?  Well we'll
	// take the same CRASH examples from above and try 'em out with references.
		// SwapWithReferences(num1, NULL); // Compiler error!  A crash is avoided
		// SwapWithReferences(num1, (int*)0x01234567); // Compiler error!  A crash is avoided

	// Well what about this?!?
	//	 SwapWithReferences(num1, 5678); // Compiler error!  A crash is avoided

	// So as you can see by uncommenting the above function calls is that using references
	// can be safer than using pointers.
	
	// Swap the numbers again
	SwapWithReferences(num1, num2);
	cout << endl << "After calling SwapWithReferences() on num1 and num2..." << endl;
	PrintNumbers(num1,num2);

	return EXIT_SUCCESS; // Tells the OS that the program exited without any problems
}

void SwapWithPointers(int *num1, int *num2)
{
	// The intent of this function is to switch the values of num1 and num2.
	// Therefore, first we'll make a temporary number to store num1
	int temp = *num1;

	// Now we'll store the value of num2 into num1
	*num1 = *num2;

	// Finally we'll put num1's value, which was saved in temp, into num2
	*num2 = temp;

} // end of SwapWithPointers(int *num1, int *num2)

void SwapWithReferences(int &num1, int &num2)
{
	//
	//	The first lets take a look at is the syntax for references.
	//
	//	SwapWithReferences(int &num1, int &num2)
	//							|
	//							|
	//							--- Notice the & then the variable name "num1".  When you see the &
	//								In a function declaration, you know that function takes a reference.
	//								This means that, the function has the potential to modify the
	//								variable and the variable must be valid.  Try to compile the 
	//								invalid examples above to prove it to yourself :) 

	// **NOTE** Unlike when a variable is passed by pointer, there is no special
	//			syntax required to use a reference inside the body of a function.
	//			You simply use the variable in the same way as is it was not passed
	//			by reference.
	int temp = num1; // Save off num1 into temp

	// Set num1 to num2
	num1 = num2;

	// Set num2 to num1's value stored in temp
	num2 = temp;

	/*	Extra reference material...
	
		Hopefully after browsing the tutorial, the concept of references is a bit clearer.  Now 
		lets talk about how using references can optimize your code.  Lets look at the following
		example:

		const int kNameLen = 255;
	
		SPlayer
		{
			char name[kNameLen];
			float hitPoints;
			float magicPoints;
			float stamina;
			float defensePoints;
		};

		We could write the following function:

			DisplayPlayerInfo(SPlayer player);

		The "problem" with the above function is that when it is called a copy
		of SPlayer would be created as it is passed to the function.

		A faster way to pass the same variable to the same function would be this:

			DisplayPlayerInfo(const SPlayer &player);
							   |				|
							   |				|-- The "&player" says we are passing a reference 
							   |					which underneath the C++ hood, passes a pointer
							   |					that will only be 32-bits (64-bit for a 64-bit OS).
							   |					When you consider that one float variable (say 
							   |					for instance "hitPoints") is 32-bits by itself, you
							   |					can start adding up the potential speed gains
							   |					from having to send less data to the function.
							   |
							   |---	Says "Hey this reference is constant and will not get modified
									by the function.  Since the compiler knows that under no 
									circumstances will the data be changed, it can potentially 
									make some optimizations.
	*/	
}

// Prints out "num1" and "num2" on separate lines
void PrintNumbers(int num1, int num2)
{
	cout << "num1 = " << num1 << endl;
	cout << "num2 = " << num2 << endl;
}

// In reference...

// References are a great way in C++ to pass data to a function and have the
// function modify it.  As for best practices, most try to be consistent with either
// using pointers or references for passing/modifying data.  Mixing the usage of the
// two in the same function or even class can be confusing.

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/