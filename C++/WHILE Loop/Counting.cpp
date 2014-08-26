//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Counting										 //
//																		 //
//		$Description:	Prints a given number range in a while loop		 //
//																		 //
//***********************************************************************//


// In this tutorial we will show how to do "loop".  A loop is something that
// "loops" and does something until we tell it to stop.  In this case we do
// a "while loop".  A while loop has a logical TRUE/FALSE statement that it
// checks each iteration of the loop to see if it evaluates to true or false.
// If the statement given to a while loop is true, it continues, otherwise
// it stops looping.  In this tutorial we have the user input a min and
// max value to print all the numbers inbetween to the screen.  In our while
// statement we say we want to loop "while min is less than or equal to max".
// Every loop we print out the min value, and then increment it by one, so
// eventually min will be greater than the max value.  Check out below how we 
// do this.


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
	cin >> min;					// We store the number they typed in the variable "min"
								// Now we prompt the user to input a number for the max
	cout << "Input your last number to count to: ";
								// Then read in the maximum number to count to and store it in the variable "max"
	cin >> max;					// Now we have the maximum number they want to count to store
								
								// Now here comes the loop.
								// This is called a "While Loop".  You will use these a tremendous amount.
								// A "While Loop" is very similar to a for loop, except there is less going on.
								// " while ( this is true ) do what ever is inside my brackets { }"
								// That is what the while loop is doing.  So in this case,
								// "while ( min is less than or equal to max ) { print out the value of min , then add 1 to min }
								// Let's say we chose 5 for min and 10 for max.
								// "while ( 5 is less than or equal to 10 ) etc....
								// This will print out 5, then 6, then 7, then 8, then 9, then 10 (it will print 10 because 10 is less than or equal to 10 )
								// Then when min becomes 11, 11 isn't <= (less than or equal to) 10, so it stops and doesn't do
								// What's in the brackets, it skips them.  No More looping
	while(min <= max)
	{							// Do what's inside these brackets until the expression is FALSE ( when min is > than max).
        cout << min << endl;	// We print out the current value of min and skip down to a new line
		min++;					// Increment min by 1.  (remember, this is the same thing as :  "min = min + 1" , just shorter.
	}							// Do what's inside these brackets until the expression is FALSE ( when min is > than max).
	
	return 0;					// Return with zero problems
}								// End the program with the last bracket for main()							


// You might be asking why you would use a "For Loop" verses a "While Loop"?
// Well, in a for loop, you specify a variable to use as the counter, then a condition for the loop, then a increment value....
// Where as , what if you don't WANT to specify all of that stuff, like what we did here?
// We could also do something like:  (remember that any text behind "//" is ignored by the compiler.

//   "while(max)				"  // This says, "while ( max does not equal 0) { print the value of max, decrement 1 from max } "
//   "{							"			
//	 "    cout << min << endl;  "  // This essentially prints the value of "max" down to zero.
//	 "	  max--;				"
//	 "}							"

// The point being that we don't always need the whole structure of a "For Loop".  We could do the
// exact same thing with a for loop, however.  In general, while loops are used when something needs to be 
// repeated while a condition is true.  For loops are used when you want to loop for a specific amount of times.

// We just completed our first while loop.  Pretending we typed in 10 and 15, here is the output:

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
