// Done by TheTutor

/*
	This tutorial is all about the keyword "extern".  So for starters, what does the "extern"
	keyword do?  The extern keyword actually can mean two different things depending on the
	context of where/how it is used.  The first usage of the extern keyword declares 
	a variable or function to have external linkage (naturally %).  This means that the 
	variable or function is "visible" from files other than the one in which it is defined.
	It is VERY IMPORANT to note that just because a variable is "externed" it does not 
	necessarily work like a "global variable".  For every file the variable is in, it can
	have a unique value(s).  However, if it's only given a value in one file, it will
	be shared across all files.

	The second usage of the extern keyword is to specify that the linkage conventions of 
	another language are being used for the declarator(s).  To better explain this example
	lets think about C and C++ code.  Lets suppose you have this code that you want to compile
	and act just like it's in a .c file.  However, you will be putting this code in a .cpp file.
	We know that .c and .cpp files compile differently (i.e. you can only declare variables at 
	the top of a function in a .c file) so it would seem that there is no way we could 
	have .c code work EXACTLY like .c code in a .cpp file.  But, by using the "extern" keyword
	you can specify certain code to use linkage conventions of a different language.  An
	example of what code using extern in this context might look like is this:

		extern "C"
		{
			char DrawSprite(int which); // Draws "which" sprite
			void ProcessSprite(int which); // Processes "which" sprite
		}

	In practice, you more than likely will NOT have to deal with the second usage of 
	the keyword "extern".  However if you're writing portable, very large projects, or hyper
	robust code you may run across it.

	This tutorial is going to focus on the first usage of the keyword "extern", and that
	is making it easy to access variable(s) across many files.  In this tutorial we are 
	going to create a dead simple point struct and two functions to access it's data.  Then
	we'll declare a externed SPoint and demonstrate how to use it.  So off to the code...

*/

#include <iostream>
using namespace std;

#include "point.h"

int main()
{
	// *** ADD CODE FROM BELOW HERE ***
	// (Read the comments under main and you'll understand %)
	
	// What do you expect this to print?  Well, we defined "externedPoint" in point.cpp
	// so it will print out the values we assigned to it there.  Notice that if we DID NOT
	// define "externedPoint" anywhere (besides point.h) we'd get a compiler error.
	cout << "Extern Point = (" << GetX(externedPoint) << "," << GetY(externedPoint) << ")";
	cout << endl << endl;

	// Here we show that yes, we can change the value of our externed point
	externedPoint.x = 50;
	externedPoint.y = 40;

	// Display the new values of "externedPoint" so we know that they did change
	cout << "Extern Point = (" << GetX(externedPoint) << "," << GetY(externedPoint) << ")";
	cout << endl << endl;
	return 0;
}

// X-turn...

/*
	The "extern" keyword is something that can be difficult to wrap your brain around.
	It is very important that if you are going to use it, you fully understand how it
	works, or you could cause yourself headaches for weeks.  Remember when you 
	declare a variable as an extern:

		extern int some_variable; // This JUST says, "Here is a variable name you can
								  // use and should know about, but the data will (must)
								  // be filled in by some other file(s)".

	Assuming "some_variable" (from directly above) is declared in a header file, all other
	files that #include that header file will be able to create a int of some_variable AND
	if you wanted to you could change the value of "some_variable" and have it be different
	for EVERY PLACE you declared it in other files.  Is that a mouthful or what!?!

	So to demonstrate this fact, in main() you'll see a comment line that reads:

		// *** ADD CODE FROM BELOW HERE ***

	Directly underneath this comment line, cut the following line of code 
	and paste it in:

		SPoint externedPoint = { 22.2f, 222.2f };

	Pay close attention to the output with and without this line of code.  By looking 
	at this example (as many times as it takes) you should learn the fundamental concept
	of the keyword "extern".	

*/

// Questions???  If so direct 'em to the forums at www.GameTutorials.com

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/

