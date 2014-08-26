// Done by TheTutor

/*	Here we're going to talk about "while loops".  A while loop is a way for your program
	to do a set of tasks WHILE some condition is true.
	
	So this program has a very simple task to complete.  We simply want the 
	user to type in the answer to "2 + 2", so our program is going to be this:

	Prompt the user to enter the answer to "2 + 2".  While they HAVE NOT entered the correct 
	answer,	keep prompting them	to enter what 2 + 2 equals.

	So with that, lets get coding :)
*/

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

// Here we include "stdio.h".  This is a header file that allows access to functions
// to do input and output.
#include <stdio.h>   // So we can use "printf()" AND "scanf()"

// Here's our "main function".  This is the core of all "console" programs (i.e. you have 
// to have a main()).  Notice how we return an "int" from main.  The rule of thumb is as
// follows:  return '1' if an error occurs in main().  If your program runs flawlessly,
// return '0' (so zero means NO errors).
int main()
{
	int answer = 0; // Here we're making a int (integer) variable and we are setting it's
				    // initial value to zero.  We will use this variable to store the integer
				    // we read in from the keyboard (user input).

	/*  Okay here it is, the while loop in all it's glory!  Notice first the keyword
		"while" (you will use this keyword to implement ALL while loops).  Now the next
		thing will look at is the stuff inside of the ()'s.

		answer != 4 -- This "condition statement" will evaluate to "true" if
					   "answer" DOES NOT equal 4.  So when we start this program
					   we know with absolute certainty that answer DOES NOT 
					   equal 4 (because we set it to zero when we create it).

					   This "condition statement" will evaluate to "false"
					   when answer DOES EQUAL 4.

		So the way a while loop works is like this: FIRST it evaluates the "condition
		statement" between the ()'s (i.e. while(condition statement) ).   If the condition
		statement is TRUE it will execute ALL the code between the {}'s.  Example:

		while(Some condition it true)
		{

			Execute EVERYTHING in here

		}

		Once we reach the "right bracket" (the '}'), we go back to the top of the while loop
		and again evaluate the "condition statement", if it's true we repeat the same process.
		If it's false, we go down to the next line of code underneath the while loop.

	*/

	// This says "while answer DOES NOT equal 4", keep looping, executing everything
	// in-between the two {}'s.
	while(answer != 4)
	{
		printf("What is 2 + 2: "); // Prompt the user (write out to the screen)
								   // for them to enter the answer to the equation "2 + 2".

		scanf("%d",&answer); // Read in WHATEVER key the user presses on the keyboard, 
							// that key will be converted to an integer. If the key is an
						   // integer (say they press the '9' key) then answer will equal 
						  // nine like you'd expect.  But say they press the 'a' key.  What
						 // would "answer" equal?  Well, answer would equal the ASCII value 
						// (i.e. an integer value) that is equivalent to 'a'.
		
		printf("\n"); // Move down to the next line (carriage return)
	}

	// The ONLY way we can get here is when the user types in 4 (thus the 
	// condition in the while loop "answer != 4" will equal FALSE and the while loop
	// will terminate).

	printf("\nYou know your math!!!\n"); // Print out a "Good job" message :)
									    // **NOTE** the '\n' simply equates
									   // to "go down to the next line".  It's like
									  // hitting "enter" on a word processor

	return 0; // Program is over so we "return"
			 // Notice we return '0' to say "No problems encountered".

} // end of main()


// If you have more questions, feel free to post them at www.GameTutorials.com

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/


	
