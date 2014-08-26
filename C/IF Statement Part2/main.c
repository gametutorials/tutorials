// Done by TheTutor

/*
	If statements are how you "make decisions" in a program. Any program that does
	any "logical branching" is more than likely going to use "if statements".
	If statements work just as you'd expect.  In practical English, an if statement might
	go like this:

		If you do not clean your room, you are going to be grounded.

	Now we make it a little more "techie":

		If this condition is true, execute this chunk of code.

	Now we go the Full Monte:

		if(num > 0)
			printf("num is positive\n");
			
	This is the whole concept behind an if statement.  Starting to make sense???  Well, read 
	on through this tutorial and hopefully they'll be crystal clear when you reach the
	bottom.
*/

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h> // We include this header file so that we can use "printf()" and "scanf()"

// Here's our main().  Remember ALL console applications MUST HAVE a main().
// The rule of thumb is if you DO NOT encounter an error in your program, return zero (0).
// If you do encounter an error, return one (1).
int main()
{
	int num; // This will hold a number

	// First thing we're going to do is prompt the user to enter a number
	printf("Enter a number: ");

	// Now we'll read in the number
	scanf("%d",&num);

	// Using "if statements" we're going to print out different text messages depending
	// on whether the number was positive, negative or zero.

	if(num > 0)  // Notice the keyword "if".  This is the syntax you use when you want
	{			// to implement an if statement:

				// if(some condition) -- If "some condition" is TRUE everything between
				// the {}'s underneath the "if statement" will be executed (if there's no
				// {}'s, then line of code underneath the if statement will get executed).
				// If "some condition" is NOT TRUE (i.e. false) NONE of the code in the {}'s
				// will be executed.

		printf("num is positive\n\n");  // Remember '\n' is just a "carriage return"
	}
	else if(num < 0) // Again notice the keywords "else if"
	{
					 // What happens here is this:  The user enters "num" (lets pretend
					// they enter -35).  First the "if" gets executed up above.
				   // -35 IS NOT greater than 0, so the program falls to this line of 
				  // code.  Then the comparison "is num less than 0" made.  This happens
				 // to be true so the code between the {}'s underneath the "else if"
				// would get executed.

		printf("num is negative\n\n");
	}
	else // Once more, please take note of the keyword "else"
	{
					// What's happening here is what you'd expect :)  If num is
				   // is not GREATER than 0 AND num is not LESS than 0, num MUST equal
				  // 0.  So this code will ONLY get executed if num equals 0.

		printf("num equals zero\n\n");
	}

	// Okay, now it's very important to note that ONLY one thing will be print to the screen
	// no matter what the user types in for num.  If the user types 50 for example, the
	// "if" will execute and then the program will "jump" to the line of code AFTER the
	// '}' in the "else" part (it will jump to the line of code:  return 0;).

	// If "num" is negative, same thing, "num is negative" will be printed to the screen
	// and the program will "jump" to the line of code "return 0;".

	// And of course, if num equals zero, the "else" will be executed and the next line
	// of code to execute is the "return 0;".

	return 0; // We did not have an errors happen in our program :)
}


// If you only knew...

/*

	Just so you are aware, here's a couple of other VERY SMALL programs that use "if"
	statements.  I don't want you to think you can only structure 'em the way we did
	in our main().

int main()
{
	if(1)					// An "if" doesn't have to be followed by ANYTHING, but 
		printf("Yeah!");	// an "else if" or "else" CAN ONLY be used in conjunction
							// with an "if".  Also notice we DO NOT have to use
							// {}'s, however if you want MUTIPLE lines of code
							// to be executed if an "if statement" is "true", then you
							// do have to use {}'s.
	return 0;
}

int main()
{
	int num;

	scanf("%d",&num);

	if(num > 90)
		printf("You got an A!\n");		// An "if" DOES NOT have to be followed
									   // with an "else if", it can be followed with	
									  // just an "else".
	else
		printf("You failed!\n");

	return 0;
}


void main()
{
	int num;

	scanf("%d",&num);

	if(num > 50)					// We HAVE to use {}'s if we only want all three printf()'s
	{								// to be executed if num is greater than 50
		printf("You\n");
		printf("are\n");
		printf("old\n");
	}

	return 0;
}

*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
