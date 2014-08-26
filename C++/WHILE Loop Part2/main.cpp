// Done by TheTutor

/*	
	Here we're going to talk about "while loops".  A while loop is a way for your program
	to do a set of tasks WHILE some condition is true.
	
	So this program has a very simple task to complete.  We want the 
	user to press the 'w' key (case sensitive).  So our program is going to be like this:

	1) Prompt user to enter the 'w' key
	2) While they HAVE NOT entered the 'w' key, keep prompting them	to enter the 'w' key.

	So with that, lets get coding :)
*/

// Here we include "iostream".  This is a header file that allows access to functions
// to do input and output (referred to as input and output streams).  The next line of code:
// "using namespace std;", says "we want to use the STANDARD input and output streams 
// provided by "iostream".

#include <iostream>   // So we can use "cout" AND "cin"
using namespace std;

// Here's our "main function".  All console (DOS style window) applications require the
// function "main".  Notice how we return an "int" from main.  This "int" is 
// used to tell the OS when the application closes. The rule of thumb is as
// follows:  return '1' if an error occurs in main(), otherwise return '0' 
// (because zero means NO errors).
int main()
{
	char letter = ' '; // Here we're making a char (character) variable and setting it
					  // equal to the space character (' ').
					 // We will use this to store the character we read in from the 
				    // keyboard.

	/*  Okay here it is, the while loop in all it's glory!.  Lets breakdown the 
		syntax initially:

		while(letter != 'w')
		 |
		 |
		 ---- The keyword "while" starts a while loop.  Code contained
			  in a pair of braces ({}'s) belongs to the while loop.

		while(letter != 'w')
					|
					|
					----  This conditional statement (letter != 'w') will evaluate to 
						  TRUE if letter DOES NOT equal 'w'.  So when we start this program
						   we know with absolute certainty that letter DOES NOT equal 'w'
						   because we set it to the space character.  Thus the first time
						   this conditional statement will evaluate to TRUE (because
						   ' ' != 'w' and the code belonging to the while loop will 
						   get executed.
	*/

	// Continue executing the code between the {}'s WHILE the
	// condition (letter != 'w') is true.
	while(letter != 'w')
	{ // Start code belonging to while loop

		cout << "Enter the letter 'w' please: "; // Prompt the user for them to enter 
												// the letter 'w'.

		cin >> letter; // Read in whatever key the user presses from the keyboard 
		cout << endl; // Move down to the next line

		// Now what happens is we loop back to the top.  The condition (letter != 'w') is
		// reevaluated.  If it is still true, we keep looping.  We will continue to
		// loop until it is false.
	
	} // End code belonging to while loop
	
	// If we get here, that means the while loop has terminated and the user
	// must have finally pressed the 'w' key.

	// Print out a "Good job" message (people love those!)
	// **NOTE** the syntax "endl" executes a carriage return
	//			It works the same as hitting "Enter" in your word processor 
	cout << endl << "You pressed 'w' !!!" << endl; 

	return 0; // Program is over so we "return"
			 // Notice we return '0' to say "No problems encountered"

} // end of main()


/*  While your still here...

	Lets recap.  Given an arbitrary while loop:

	while(Some condition it true)
	{
		Execute CODE in here
	}
	
	1) FIRST it evaluates the conditional statement between the parenthesizes ()'s.
	2) If it is true, the while loop is entered, otherwise code execution
	   continues at the next line of code after the while loop
	3) All the code between the brakets {}'s is executed
	4) When the bottom bracket } is reached, the code jumps back to item #2
*/

// While questions are on your mind, be sure to post 'em at www.GameTutorials.com

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/

	
