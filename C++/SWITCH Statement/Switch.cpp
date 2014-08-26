//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		DigiBen@gametutorials.com			 //
//																		 //
//		$Program:		Switch Statements								 //
//																		 //
//		$Description:	Shows the applications for switch statements.	 //
//																		 //
//***********************************************************************//


// Below we go over using a "switch statement".  A switch statement is like
// using an if/else statement, except the format is different.  You can do
// more with if/else statements like check strings, wherease switch statements
// are generally good for just checking integer and char values.  Let's go 
// through the tutorial and see how we use switch statements.


#include <iostream>							// This line of code is necessary to include all the C++ standard functions\classes definitions
using namespace std;						// Set our namespace to standard (don't stress on this right now)

int main()									// The beginning of our program
{											// This starts the main function
	int number = 0;							// Initialize an integer to hold a number
	char ch = 0;							// Initialize a variable to hold a character

											// Prompt the user to enter a number from 1 to 3
	cout << "Input a number between 1 and 3: ";																					
	cin >> number;							// Read in the number from the keyboard 

	// The code below is a switch statement.  Basically, what it does
	// is do a bunch of if() statements inside.  In English, the code below is
	// saying, "Check the value of number, if it's a 1, say so, if it's
	// a 2, say so, if it's a 3, also say so.  However, if it's none of these,
	// then tell the user they didn't follow directions."

	// You put the variable you want to evaluate between the parenthesis (<here>).
	// Then you create a new scope (The { } ).  The "case" keyword is the same
	// thing as saying, "If number's value equals 1".  Or, relating to an if() statement:
	// "if(number == 1)". We put a colon after the case.  Now, if the value of "number:
	// is 1, it will do everything after the case 1: and between the "break";
	// In this case, we just print to the screen they typed the number 1, and then
	// we "break" out of this switch statement.  What the "break" does, is leave the
	// scope of the switch statement.  So, it will go to the next line AFTER the ending '}'.
	// If we didn't have a "break" it would go to the next line, which happens to be "case 2:".
	// Once one case is evaluated to be true, then it continues to do the ones after that
	// unless there is a break. This is strange, but useful, as seen in an example below this one.

	// If none of the statements are evaluated to TRUE, then it goes down to the "default".
	// You don't have to have a default, but it's usually a good idea.

	switch(number)							// Check the value of "number"
	{										// The beginning of the switch statements scope
		case 1:								// If number equals 1, say so
			cout << "You typed in the number 1!\n";
			break;							// Stop this and leave the switch statement

		case 2:								// If number equals 2, say so
			cout << "You typed in the number 2!\n";	
			break;							// break out of the switch statement

		case 3:								// If number equals 3, say so
			cout << "You typed in the number 3!\n";
			break;							// break from this switch statement
									
		default:							// If the number isn't 1, 2 or 3, complain :)
			cout << "You didn't follow directions!\n";
			break;							// We don't need the break really, but I like to for consistency.
											// we will leave the switch statement anyway, that's why.
	}										// This is the END of the switch statements scope

	// Below, we will show you an example of checking characters...

	cout << "\n\nInput a character: ";		// Ask for the user to input a character																		
	cin >> ch;								// Read in a character from the keyboard

	switch(ch)								// Check the value of "ch"
	{										// The beginning of the switch statements scope
		case 'a':							// If ch equals 'a', say so
			cout << "You typed in an A!\n";
			break;							// Stop this and leave the switch statement

		case 'b':							// If ch equals 'b', say so
			cout << "You typed in a B!\n";	
			break;							// break out of the switch statement

		case 'c':							// If ch equals 'c', say so
			cout << "You typed in a C!\n";
			break;							// break from this switch statement
									
		default:							// If ch isn't 'a', 'b' or 'c', let us know
			cout << "You did not type an a, b or c!\n";
			break;							// Once again we don't need this break, but it doesn't hurt.											
	}										

	// Well, you will notice that the last example only checks for LOWER CASE characters.
	// We don't want to have to make a whole new case for lower and upper case characters,
	// so here is what we do: we just add another case right after the first one, separated
	// by a colon (:).   We can do as many as we want.  We can also do it on each line, like this:
	// case 'a':
	// case 'A':
	//	cout etc...

	cout << "\n\nInput a lowercase or uppercase character: ";		
	cin >> ch;								// Read in a character from the keyboard

	switch(ch)								// Check the value of "ch"
	{										// The beginning of the switch statements scope
		case 'a': case 'A':					// If ch equals 'a' or 'A', say so
			cout << "You typed in an A!\n";
			break;							// Stop this and leave the switch statement

		case 'b': case 'B':					// If ch equals 'b' or 'B', say so
			cout << "You typed in a B!\n";	
			break;							// break out of the switch statement

		case 'c': case 'C':					// If ch equals 'c' or 'C', say so
			cout << "You typed in a C!\n";
			break;							// break from this switch statement
									
		default:							// If ch isn't a, b or c, let us know
			cout << "You did not type an a, b or c!\n";
			break;							// Once again we don't need this break, but it doesn't hurt.											
	}										
	
	return 0;								// Return with zero problems
}											// This ends the main function (program is done)

///////////////////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES *
//
// Switch statements are a LOT cleaner than if() statements when is comes to checking
// more than 2 or 3 values.  It is customary to use a switch statement when you have
// over 2 values to check, otherwise if() statements can get too messy.  There is some
// restrictions on switch statements though.  One of them is that it can't do strings.
// You can not do :  switch("Test") or switch(strName).  The reason is, is because a
// string is an array, and switch() statements just check a constant value.  Switch()
// statements can't do floating point numbers either, so no: switch(22.2).  This is
// unfortunate too.  The last thing that restricts switch statements is that you can't
// use expressions with the case, such as:  case (number != 2):
//
// Otherwise, switch statements are great!
//
// © 2000-2005 GameTutorials
