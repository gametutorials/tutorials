// Done by TheTutor

/*
	If statements are how you "make decisions" in a program -- Any program that does
	any "logical branching" is more than likely going to use "if statements" --
	If statements work just as you'd expect.  In practical English, an if statement might
	go like this:

		If you do not clean your room, you are going to be grounded.

	Now we make it a little more "techie"

		If this condition is true, execute this chunk of code.

	Now we go the Full Monte --

		if(num > 0)
			cout << "num is positive" << endl;
			
	This is the whole concept behind an if statement - starting to make sense???  Well read 
	on through this tutorial and hopefully it will be crystal clear when you reach the
	bottom.

*/

#include <iostream> // We include this header file so that we can use "cout" and "cin"
using namespace std; // Here we make sure we use the standard namespace (again this 
				    // allows us to use "cout" and "cin").

// Here's our main() -- Remember ALL console applications MUST HAVE a main()
// The rule of thumb is if you DO NOT encounter an error in your program, return zero (0)
// If you do encounter an error, return one (1)
int main()
{
	int num; // This will hold a number

	// First thing we're going to do is prompt the user to enter a number
	cout << "Enter a number: ";

	// Now we'll read in the number
	cin >> num;

	// Using "if statements" we're going to print out different text messages depending
	// on whether the number was positive, negative or zero.

	if(num > 0)  // Notice the keyword "if" -- This is the syntax you use when you want
	{			// to implement an if statement:

				// if(some condition) -- If "some condition" is TRUE everything between
				// the {}'s underneath the "if statement" will be executed (if there's no
				// {}'s, then line of code underneath the if statement will get executed).
				// If "some condition" is NOT TRUE (i.e. false) NONE of the code in the {}'s
				// will be executed.

		cout << "num is positive" << endl << endl;
	}

	else if(num < 0) // Again notice the keywords "else if"
	{
					 // What happens here is this:  The user enters "num" (lets pretend
					// they enter -35).  First, the "if" gets executed up above.
				   // -35 IS NOT greater than 0, so the program falls to this line of 
				  // code -- Then the comparison: "is num less than 0" is made -- This happens
				 // to be true, so the code between the {}'s underneath the "else if"
				// would get executed

		cout << "num is negative" << endl << endl;
	}
	else // Once more, please take note of the keyword "else"
	{
					// What's happening here is what you'd expect :) -- If num is
				   // not GREATER than 0 AND num is not LESS than 0, num MUST equal
				  // 0 -- So this code will ONLY get executed if num equals 0

		cout << "num equals zero" << endl << endl;
	}

	// Okay now it's very important to note that ONLY one thing will be print to the screen
	// no matter what the user types in for num.  If the user types 50 for example, the
	// "if" will execute and then the program will "jump" to the line of code AFTER the
	// '}' in the "else" part (it will jump to the line of code:  return 0;)

	// If "num" is negative, same thing -- "num is negative" will be printed to the screen
	// and the program will "jump" to the line of code "return 0;"

	// Of course if num equals zero, the "else" will be executed and the next line
	// of code to execute is the "return 0;" 

	return 0; // We did not have any errors happen in our program :)
}

// If you only knew...

/*

	Just so you are aware, here's a couple of other VERY SMALL programs that use "if"
	statements.  I don't want you to think you can only structure 'em the way we did
	in our main()

int main()
{
	if(1)					// An "if" doesn't have to be followed by ANYTHING -- But 
		cout << "Yeah!";	// an "else if" or "else" CAN ONLY be used in conjunction
							// with an "if" -- Also notice we DO NOT have to use
							// {}'s, however if you want MULTIPLE lines of code
							// to be executed if an "if statement" is "true", then you
							// do have to use {}'s.
	return 0;		
}

int main()
{
	int num;

	cin >> num;

	if(num > 90)
		cout << "You got an A!" << endl;		// An "if" DOES NOT have to be followed
											   // with an "else if" it can be followed with	
											  // just an "else".
	else
		cout << "You failed!" << endl;
		
	return 0;
}


int main()
{
	int num;

	cin >> num;

	if(num > 50)					// If we HAVE to use {}'s if we want all three
	{								// cout's to be executed if num is greater than 50.
		cout << "You" << endl;
		cout << "are" << endl;
		cout << "old" << endl;
	}
	
	return 0;
}

*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
