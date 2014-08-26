/*	Done by TheTutor

	This tutorial focuses on "binary operators".  The binary operators we are going to 
	talk about are  AND (denoted by a '&' in code)
					OR  (denoted by a '|' in code)
					XOR "exclusive or" (denoted by a '^' in code)

	Okay, now if you don't know what binary numbers are, your up a creek!  Just kidding 
	of course.  So first, a quick lesson on binary numbers.

	Now first we need to give the "TRUE" mathematical definition of base10 numbers (these are 
	the ones you use all the time -- 1, 2, 3...)  Oh, and first a little notation:  I'm going to 
	treat the '^' to mean "raise to the power of" -- So 2^2 = 4 (two squared)  Got it? :)

	The base10 system works like this;  If we have the number, say 21 for example, what we are
	really saying is this:

	10^4	10^3	10^2	10^1	10^0

     0		 0		 0		 2		 1

	This says we have (2 * 10^1) + (1 * 10^0) units -- A little algebra:
	(2 * 10) + (1 * 1) = 21

	Does this make sense???  Look at it closely and it should.  Squint if you have too :)
	Now binary numbers work the same way.  Except instead of being base10, they are base2.

	So now we'll represent the number 21 in binary

	2^5		2^4		2^3		2^2		2^1		2^0

	 0		 1		 0		 1		 0		 1

	This says we have (0 * 2^5) + (1 * 2^4) + (0 * 2^3) + (1 * 2^2) + (0 * 2^1) + (1 * 2^0)
	Algebra goes here:  0 + 16 + 0 + 4 + 0 + 1 = 21

	Pretty simple huh???

	Okay now that your a binary numbers expert, let's move on...

	Let's use the numbers 5 and 6 as examples.  First, we represent 5 and 6 as binary numbers.

	101 = 5
	110 = 6

	The AND operation works like this:	0 AND 0 equals 0
										0 AND 1 equals 0
										1 AND 0 equals 0
										1 AND 1 equals 1


	So just like you add two numbers, you go column by column from right to left (you can also go left to right).

	101
	110	 AND
	--- 
	100 = 4				So 5 & 6 = 4 -- Excellent, you are now our a master of the AND operator


	************************************************************************************************

	The OR operator -- Again will use 5 and 6 as our numbers.

	The OR operation works like this:   0 OR 0 equals 0
										0 OR 1 equals 1
										1 OR 0 equals 1
										1 OR 1 equals 1

	Thus:

	101
	110  OR
	---
	111 = 7		So 5 | 6 = 7 -- You have now become a MASTER at the OR operator


	************************************************************************************************

	Last, but not least the XOR (exclusive or) operator -- Again our numbers will be 5 and 6

	The XOR operation works like this:	0 XOR 0 equals 0
										0 XOR 1 equals 1
										1 XOR 0 equals 1
										1 XOR 1 equals 0

	Thus:

	101
	110  XOR
	---
	011 = 3		So 5 ^ 6 = 3 -- Still not a master of the XOR :)


	/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

	I've put a handy dandy "truth table" at the bottom of this .c file.
	Now on with a few more examples and a couple practical uses :)

	*/

#include <stdio.h> // We can use printf()

int main()
{
	int num1 = 8;
	int num2 = 3;

	int answer = 6; // We'll store our answers in here

	// Okay, for your sake I would suggest FIGURING OUT ON PAPER what should be printed out
	// once you do it.  That way you won't forget :)

	// Print out the result of num1 AND num2
	answer = num1 & num2;
	printf("%d & %d = %d\n\n",num1,num2,answer);

	// Print out the result of num1 OR num2
	answer = num1 | num2;
	printf("%d | %d = %d\n\n",num1,num2,answer);

	// Print out the result of num1 XOR num2
	answer = num1 ^ num2;
	printf("%d ^ %d = %d\n\n",num1,num2,answer);

	// Print out the result of (num1 AND num2) XOR (num1 OR num2)
	answer = (num1 & num2) ^ (num1 | num2);
	printf("(%d & %d) ^ (%d | %d) = %d\n",num1,num2,num1,num2,answer);

	return 0; // End of program

} // end of main()


// Practical uses -----

	/* Well for starters, the fastest way to set a variable to zero is to XOR it with itself.
	   So if I have: int an_int = 7;

		...and I want to set it to zero I could go:

			an_int = an_int ^ an_int; -- The short-hand version: an_int ^= an_int;

		An ALL MIGHTY compiler would switch every "an_int = 0;" into "an_int ^= an_int".
		VC++ is pretty good about this.

		Another example is, say if you want to know if a number is odd,
		this piece of code will do it for ya:

		if((a_number & 1) != 0)
			// The number is odd
		else
			// The number is even

	*/


	/* LET THE TRUTH BE KNOWN:

		Here's the truth table:

			AND	 OR	 XOR
		00|  0    0   0
		-------------------
		01|  0    1   1
		-------------------
		10|  0    1   1
		-------------------
		11|  1    1   0
		-------------------

	*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
