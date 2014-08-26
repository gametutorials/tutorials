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


// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>

void main()									// The beginning of our program
{
	int min=0, max=0, i=0;					// We initialize 3 integer (number) variables to store
											// our range.  We set them equal to zero, or else they
											// will be some crazy number like -858993460.  We could
											// Also have said:
											//					int min=0;
											//					int max=0;
											//					int i=0;
											// They are the same thing, it is just cleaner the other way

											// Now we prompt the user to input a number for the min:
	printf("Input your first number to count from: ");
											// Then we wait until they press <enter> and read what
											// they typed in, then storing it in the variable "min".
											// We use "%d" because that means we are reading in an integer (number).
	scanf("%d", &min);						// See the "&"?  That means we are putting what ever is typed in, at the memory address of the variable.
											// Here is how this works, it is kinda confusing.  When a variable is allocated,
											// and when I say allocated, we tell windows that we will need a certain amount of memory from the system.
											// When Windows responds, it gives us a memory location to store the info for that variable.
											// We always want to use the least amount of memory.  For small programs like this it isn't important,
											// but when you get into HUGE projects, it is of UTMOST importance.  It goes fast.  Trust me.
											// So, generally we only want to use the LEAST amount of variables as possible to conserve memory.
											// This is where you start to determine a good programmer:  The fastest and most efficient way.
											// We all sometimes struggle with this.  There seems to always be a better way.
											// Now we prompt the user to input a number for the max.
	printf("Input your last number to count to: ");
											// We then read in the highest number
	scanf("%d", &max);
											// Now here comes the loop:
											// This is called a "For Loop".  You will use these a million times.
											// Ok, here is what it does.  It gets a starting point:
											//                "i=min"     "i" is used as a counter
											// "i" now equals min, let's say we typed in 10.
											// This is the same thing as saying "i=10", but we don't
											// know WHAT the number is so we hold it in a variable: min
											//				  "i <= max"	This says, keep looping until this condition is false.
											// In this case, "keep looping until i is greater than max".
											// Let's pretend that max is 15.  The loop will quit when i = 16 or over.
											// You might be thinking, why would i = 16 or over?  "i" equals "min" (let's say 10).
											// Well, the next parameter passed into the "for" loop is where we say what happens to "i"
											//				   "i++"		This tells the compiler to add 1 to "i" each time through.
											// This is the same thing as saying : "i = i + 1", It's just shorthand.
//	for(start, condition for the loop, after we go through the loop each time - do this) )
											// You'll also notice we don't have a ";" after the "for" loop...
											// Good "i"! :)  if we put a semicolon after this, the loop would never run.
	for(i=min; i <= max; i++)				// That is because a ";" says we are done with the line, we ARENT... We never put a ";" after anything with a "{" after it .. look at "void main()" ..
	{										// We have a "{" to say everything after "{" is in the loop
		printf("%d\n", i);					// "Printf" works just like "scanf()", instead of reading, we are printing out.. and going to the next line with "\n".
											// In this case, we use "%d" to print out a number, which is "i" as we see in the next parameter.
	}										// Everything after "}" is out of the loop, if the loop is not finished, go back to the top.

											// You might be wondering what this does?  If so.. let's go through the loop:
											// Let's stick to our previous example of min=10 and max=15.
											//		for(i=10, 10 <= 16, 10 + 1 (only if middle condition is not met, so i still = 10) )
											//		{
											//			printf("%d\n", 10);
											//		}
											// That is one time through the loop, when it hits "}" .. The loop executes the 3rd parameter,
											// which happens to be "i++".  Now, i = 11 right?  We just added 1 to i which was 10.
											// The compiler never goes back to the first parameter which was "i=min". That was just to start "i" off.
											// So, after the first loop, and we add 1 to "i", the compiler goes and check the middle condition
											// to see if the loop should continue.. So:
											//				"11 <= 16"  It's still TRUE, so we go through the loop again.
											// Now we print out 11, then 12 <=16, then print 12, etc...
											// until we print out "16"  , after the loop ends, 16 gets 1 added to it. i = 17
											//				"17 <= 16"  This is FALSE, so the loop quits and the code 
											// goes past the loop and past the "}".  Since there is nothing else in the program, the program is over.
}											// We end "main()" ending our program

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


// © 2000-2005 GameTutorials
