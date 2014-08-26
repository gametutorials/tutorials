//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Counting										 //
//																		 //
//		$Description:	Prints a given number range in a for loop		 //
//																		 //
//***********************************************************************//


// In this tutorial we will show how to do "loop".  A loop is something that
// "loops" and does something until we tell it to stop.  In this case we do
// a "for loop".  A for loop has us specify a start, and end, and an inbetween
// statement.  So, if we want to print out the numbers 1 through 100 to the
// screen, do we really want to write a line of code for each number?  Of
// course not, so we use a loop saying we want to start at 1, stop at 100,
// and increase our number by 1.  Check out below how we do this.


#include <iostream>				// This line of code is necessary to include all the C++ standard functions\classes definitions
using namespace std;			// Set our namespace to standard (don't stress on this right now)

int main()						// This creates our function main()
{								// Beginning of the program
	int min=0, max=0, i=0;		// We initialize 3 integer (number) variables to store
								// our range.  We set them equal to zero or else they
								// will be some crazy number like -858993460.  We could
								// also have said:
								//					int min=0;
								//					int max=0;
								//					int i=0;
								// They are the same thing, it is just cleaner the other way

								// Now we prompt the user to input a number for the min
	cout << "Input your first number to count from: ";
								// Then we wait until they press <enter> and read what they typed in
	cin >> min;					// We store the number they type in in the variable "min"
								// Now we prompt the user to input a number for the max
	cout << "Input your last number to count to: ";
								// Then read in the maximum number to count to and store it in the variable "max"
	cin >> max;					// Now we have the maximum number they want to count to store

								// Now here comes the loop:
								// This is called a "For Loop".  You will use these a million times.
								// Keep in mind that there are essentially three lines of code between "for(" and ")".
								// Each line is separated by the semicolon.  This is normal for "for loops".
								// Ok, here is what it does.  It gets a starting point
								//                "i=min"     "i" is used as a counter
								// "i" now equals min, let's say we typed in 10.
								// This is the same thing as saying "i=10", but we don't
								// Know WHAT the number is so we hold it in a variable: min
								//				  "i <= max"	This says, keep looping until this condition is false. 
								// In this case, "Keep looping until i is greater than max."
								// Let's pretend that max is 15.  The loop will quit when i = 16 or over.
								// You might be thinking, why would i = 16 or over?  "i" equals "min" (let's say 10)
								// Well, the next parameter passed into the "for loop" is where we say what happens to "i".
								//				   "i++"		This tells the compiler to add 1 to "i" each time through.
								// This is the same thing as saying : "i = i + 1", It's just shorthand.  The loop in words:
								// for(start; condition for the loop; after we go through the loop each time - do this)
								// You'll also notice we don't have a ";" after the "for" loop...
								// Good "i"! :)  if we put a semicolon after this, the loop would never run.
								// That is because a ";" says we are done with the line, but we ARENT... 
	for(i=min; i <= max; i++)	// We never put a ";" after anything with a "{" after it .. look at "int main()" ..
	{							// We have a "{" to say everything after "{" is in the loop
		cout << i << endl;		// Here we print out "i" each time through the loop and go to the next line							
	}							// Everything after "}" is out of the loop, if the loop is not finished, go back to the top of the loop

								// You might be wondering what this does?  If so.. let's go through the loop:
								// Let's stick to our previous example of min=10 and max=15
								//		for(i=10, 10 <= 16, 10 + 1 (only if middle condition is not met, so i is still = 10) )
								//		{
								//			cout << i << endl;		// i is 10 right now (the first iteration of the loop)
								//		}
								// That is one time through the loop.  When it hits "}" .. the loop executes the 3rd statement,
								// which happens to be "i++".  Now, i = 11 right? we just added 1 to i which was 10.
								// The compiler never goes back to the first parameter which was "i=min". That was just to start "i" off.
								// So, after the first loop, and we add 1 to "i", the compiler goes and checks the middle statement
								// to see if the loop should continue.. So:
								//				"11 <= 16"  ... It's still TRUE, so we go through the loop again.
								// Now we print out 11, then "12 <= 16" , then print 12, etc...
								// until we print out "16".  After the loop ends, 16 gets 1 added to it: now i = 17.
								//				"17 <= 16"  ... This is FALSE, so the loop quits and the code 
								// goes past the loop and past the "}".  Since there is nothing else in the program, the program is over.

	return 0;					// Return zero for "zero problems in this program"
}								// We end "main()" ending our program

// We just completed our first for loop.  Pretending we typed in 10 and 15, here is the output:

// Input your first number to count from: <we type in 10 and press ENTER>
// Input your last number to count to:	  <we type in 15 and press ENTER>
// 10
// 11
// 12
// 13
// 14
// 15
// Press any key to continue				// Windows way of saying "Get out of my program!" :)											
	
// One cool thing about C++ that's different from C is that you can create
// A variable on the fly, like this:  for(int i = min; etc.....)
// In C you have to declare all variables at the top of the function.

// © 2000-2005 GameTutorials
