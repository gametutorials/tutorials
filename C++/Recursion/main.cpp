// Done by TheTutor

/*	Okay so what the heck is recursion?  Well it's NOT a C/C++ thing.  Recursion can be
	implemented in any language.  Ever get a lecture from your father? That's recursion :)
	Recursion is a logical process, that when used at the correct times, can help accomplish 
	a task faster and/or with less code.

	Recursion is simply defined as having a function that calls itself inside of the function.
	Did I lose you?  Okay VERY QUICK example of recursion that happens to go on forever and
	would eventually crash your computer...

	void badRecursion()
	{
		cout << "This will eventually crash" << endl;

		badRecursion();
	}

	As you can hopefully more clearly see now, recursion is nothing more than a function
	that calls itself inside of the function's implementation.  So it is sorta like your
	dad telling you the same thing over and over, until you finally get it, then he can
	stop telling ya.

	So for a GOOD example of recursion we are going to implement a factorial function
	Let's do a quick mathematical example so we're all on the same page.
	
	First things, the ! symbol in the math world means "factorial".  So 3! is read
	"three factorial".  When you take the factorial of a number you take all the whole numbers
	that precede it, starting at 1, and multiply them together.  So...
	
	3! = 1 * 2 * 3 = 6

	Okay lets make our factorial function using recursion.

*/

// Standard include to use cout -- alternative (though older) would be this:
// #include <iostream.h> -- Then you'd have to remove the using namespace std; line
#include <iostream>
using namespace std;

typedef unsigned int uint; // We're lazy so instead of typing unsigned int, we'll just
						   // make it so we only have to type uint for unsigned integers.
						   // A typedef basically creates a macro for variables.  When
						   // we create a variable of "uint", it will be a shorter version of 
						   // saying "unsigned int myInt".  "uint" is our created variable name.  An
						   // unsigned int means that it can only be a positive number, no negative 
						   // numbers.  "signed" int is the default "int" which can have negative numbers.

// Our killer factorial function - this is the prototype. The function is defined below main().
uint Factorial(uint num);

// Our main is small and fit like a super model.  
// It should print out the number 6 and then be done with it's sexy self.
int main()
{
	cout << Factorial(3) << endl;
		return 0;
}

// All right this is it! 
// Let's think about the definition of factorial for a second.
// We said 3! = 1 * 2 * 3 which is equivalent to 3 * 2!, We know 1! = 1, so by using
// those simple facts we design our factorial function to mimic these properties.
uint Factorial(uint num)
{
	// Factorial of one is one,  thus we'll return one
	if(num == 1)
		return 1;

	else
		return num * Factorial(num - 1); // Otherwise it's num * the factorial of (num - 1)
}

/*	So lets go through step by step what happens when Factorial(3) gets called:

	Factorial(3)

	// "num" doesn't equal 1 so we return num * Factorial(2), but before 
	// we can return, Factorial(2) must return

		Factorial(2)

		// "num" doesn't equal 1 so we return num * Factorial(1) but before 
		// we can return, Factorial(1) must return

			Factorial(1)

			// Here num DOES EQUAL 1 so we return 1

		// Now we're back in "Factorial(2)" and we're at the line of code
		// "return num * Factorial(1)".  Now remember in Factorial(2) "num" EQUALS 2
		// So what we have is num equals 2 and Factorial(1) has returned 1 so 
		// the call to Factorial(2), returns 2 (The result of 2 * 1)

	// Now we're back in "Factorial(3)" and we're at the line of code 
	// "return num * Factorial(2)".  Now remember in Factorial(3) "num" EQUALS 3.
	// So what we have is num equals 3 and Factorial(2) has returned 2 so 
	// we return 6 (The result of 3 * 2).

	// **NOTE** every time you call a function you get LOCAL COPIES of the 
	// variable(s) passed in.  That means these local copies ARE ONLY KNOWN to that
	// specific function call.  So, when we pass in "num" into a function, we aren't
	// actually passing in the variable num, just the current value of it.
*/

// As always, any concerns, comments, criticisms, and tasty recipes can
// be posted at www.GameTutorials.com :)

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
	
