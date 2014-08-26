// Done by TheTutor

#include <stdio.h>	// So we can use printf()
#include <stdlib.h> // Defines EXIT_SUCCESS
#include <conio.h> // So we can use kbhit()

/*	This tutorial is going to cover the do while loop.  Do while loops are very similar to
	while loops, the ONLY real difference is that do while loops are guaranteed to execute
	at least once.

	Do while loops look like this:

	do
	{
		// Execute lines of code
	
	} while(// Some expression is true);
	
	**NOTE**  The command "break", "goto" or "return" will 
			  prematurely exit (stop the loop and begin at the first 
			  line of code following the loop) a do while loop.

*/

// This main is pretty simple.  We're going to declare an integer and WHILE it's value is less
// than 10 we're going to print the integer to the screen.
int main()
{
	int num = 0; // This is our "counting" number.  We'll initially set it to zero.


	do // Here we begin our do while loop
	{
		printf("%d\n",num); // This prints "num" to the screen and then advances to the next line.
						    // **NOTE**	The "\n" works just like pressing "Enter" in NotePad.
							// It stops printing on the current line and moves where
							// the next character will be print to the beginning of the line
							// beneath (like pressing return on a typewriter).

		num++;				// The "++" increments num by one.  So if num equalled 6, after 
							// the line of code num++; was executed, num would equal 7.
	
	} while(num < 10); // This is the while (condition) part of our loop.  This loop
					   // will continue while num is less than 10.  The main difference
					   // from a "normal while loop" is the check to see if the loop should
					   // stop happens at the END of the loop instead of at beginning.
					   // Example:

					   /* 
							while(num < 10) // Check is at beginning
							{
					   		 num++;
							}

							do
							{
							  num++;

							} while(num < 10); // Check is at end

						**NOTE**  Be sure to notice how the "do while" has a semi-colon after
								  the while part.
						*/

	return EXIT_SUCCESS; // The program terminated correctly

} // end of main()


/*
	Do while loops are typically used the least of the "looping commands"  (for, while).
	They are best used when you KNOW you have to do something at least ONCE, but potentially
	many times.

	If you have any questions be sure to post 'em at www.GameTutorials.com
*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
