// Done by TheTutor

/*	Okay so what the heck is recursion?  Well it's NOT a C/C++ thing.  Recursion can be
	implemented in "any language".  Recursion is more of a logical process, that when
	used at the correct times, can help accomplish a task faster with less code.

	Recursion is simply defined as having a function that calls itself inside of the function.
	Did I lose you?  Okay VERY QUICK example of recursion (that happens to go on forever and
	would eventually crash your computer)

	void badRecursion()
	{
		printf("This eventually crashes\n");

		badRecursion();
	}

	As you can hopefully more clearly see now, recursion is nothing more than a function
	that calls itself inside of the function's implementation.

	So for a GOOD example of recursion.  We're going to implement a factorial function.
	Let's do a quick mathematical example (so we're all on the same page)
	
	First things, the ! symbol (in the math world) means "factorial".  So 3! is read
	"three factorial".  When you take the factorial of a number you take all the whole numbers
	that precede it (starting at one) and multiply them together.  So...
	
	3! = 1 * 2 * 3 = 6

	Okay let's make our factorial function using recursion.

*/

#include <stdio.h>

typedef unsigned int uint; // We're lazy so instead of typing unsigned int, we'll just
						  // type uint.  An unsigned int is a int that can't be negative
						 // so we can double the number capacity by saving the bit used for negative.

// Our killer factorial function
uint Factorial(uint num);

// Our main is pretty weak.  It should print out the number 6 and then be done with
// it's bad self.
int main()
{
	printf("%d\n",Factorial(3));

	return 0;
}


// Alright this is it -- 
// Let's think about the "definition" of factorial for a second.
// We said 3! = 1 * 2 * 3, which is equivalent to 3 * 2!.  We know 1! = 1, so by using
// those simple facts we design our "factorial function" to mimic these properties.
uint Factorial(uint num)
{
	// Factorial of one is one, thus we'll return one
	if(num == 1)
		return 1;

	else
		return num * Factorial(num - 1); // Otherwise it's num * the factorial of (num - 1)
}

/*	So lets go through step by step what happens when factorial(3) gets called:

	Factorial(3)

	// "num" doesn't equal 1 so we return num * Factorial(2,) but before 
	// we can return, Factorial(2) must return.

		Factorial(2)

		// "num" doesn't equal 1 so we return num * Factorial(1), but before 
		// we can return, Factorial(1) must return

			Factorial(1)

			// Here num DOES EQUAL 1 so we return 1

		// Now we're back in "Factorial(2)" and we're at the line of code
		// "return num * Factorial(1)".  Now remember in Factorial(2) "num" EQUALS 2
		// So what we have is num equals 2 and Factorial(1) has returned 1 so 
		// the call to Factorial(2), returns 2 (The result of 2 * 1).

	// Now we're back in "Factorial(3)" and we're at the line of code 
	// "return num * Factorial(2)".  Now remember in Factorial(3) "num" EQUALS 3.
	// So what we have is num equals 3 and Factorial(2) has returned 2, so 
	// we return 6 (The result of 3 * 2).

	// **Remember** every time you call a function you get LOCAL COPIES of the 
	// variable(s) passed in.  That means these "local copies" ARE ONLY KNOWN to that
	// specific function call.
*/

// As always, any concerns, comments, criticisms or opinion donations can
// be posted at www.GameTutorials.com :)

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/	
